#!/usr/bin/python3
"""
Helper function to create thing and add keys to project
1. Create certificates
2. Create thing
3. Attach certificate to thing
4. Creat and attach policy to thing
5. Retrieve and write endpoint to file
"""

from esptool import ESP32ROM
import boto3
import argparse
from shutil import copyfile
import sys

# Change these as per your requirements
thing_type_name = "Testtype"
owner_name = "owner_name"
policy_name = "PubSubToAny"

def get_deviceId(mac):
    return ''.join(map(lambda x: '%02x' % x, mac))

try:
    esp32 = ESP32ROM()
    esp32.connect()
    mac = esp32.read_mac()
    deviceId = get_deviceId(mac).lower()
    print("Generating certificates and barcode for device with ID: {}".format(deviceId.upper()))

    client = boto3.client('iot', region_name='ap-south-1')
    
    # Create policy: Set as required.
    # To learn more about AWS IoT policies, visit https://docs.aws.amazon.com/iot/latest/developerguide/iot-policies.html
    try:
        response = client.create_policy(
                                        policyName=policy_name,
                                        policyDocument = '{"Version": "2012-10-17","Statement": [{"Effect": "Allow","Action": ["iot:*"],"Resource": ["*"]}]}'
                                        )
    except Exception as e:
        # Policy already exists
        # print ("Exception raised with the message: {}".format(str(e)))
        print ("Reusing existing policy.")

    #delete existing certificates
    
    try:
        response = client.list_thing_principals(thingName=deviceId)
        for principal in response['principals']:
            print(principal)
            principalId = principal.split('/')[1]
            print(principalId)
            response = client.detach_thing_principal(
                                                    thingName=deviceId,
                                                    principal=principal
                                                    )
            response = client.detach_policy(
                                        policyName=policy_name,
                                        target=principal
                                        )
            response = client.update_certificate(
                                                certificateId=principalId, newStatus='INACTIVE'
                                                )
            response = client.delete_certificate(
                                            certificateId=principalId,
                                            forceDelete=True
                                            )
    except:
        pass

    # Get certificates and keys
    certs = client.create_keys_and_certificate(setAsActive=True)
    certificateArn = certs["certificateArn"]
    certificateId = certs["certificateId"]
    certificatePem = certs["certificatePem"]
    privateKey = certs["keyPair"]["PrivateKey"]
    publicKey = certs["keyPair"]["PublicKey"]

    with open("aws_credentials/{}.certificate.pem.crt".format(deviceId),"w") as f:
        f.write(certificatePem)

    with open("aws_credentials/{}.private.pem.key".format(deviceId),"w") as f:
        f.write(privateKey)

    with open("aws_credentials/{}.public.pem.key".format(deviceId),"w") as f:
        f.write(publicKey)

    # Attach policy to certificate
    response = client.attach_principal_policy(
                                              policyName=policy_name,
                                              principal=certificateArn
                                              )

    # Create thing
    try:
        response = client.create_thing(thingName=deviceId, thingTypeName=thing_type_name, attributePayload={
                               "attributes": {"owner": owner_name}})
    except:
        response = client.delete_thing(thingName=deviceId)
        response = client.create_thing(thingName=deviceId, thingTypeName=thing_type_name, attributePayload={
                                       "attributes": {"owner": owner_name}})
    # Attach certificate to thing
    response = client.attach_thing_principal(
        thingName=deviceId,
        principal=certificateArn
    )

    # # copy AWS certificates to SPIFFS image
    copyfile("aws_credentials/{}.certificate.pem.crt".format(deviceId), "spiffs_image/certificate.pem.crt")
    copyfile("aws_credentials/{}.private.pem.key".format(deviceId), "spiffs_image/private.pem.key")

    with open("mac_address.txt","w") as f:
        f.write(deviceId.upper())

    print ("\nAll done. Certificates are in aws_credentials/ directory.\n")

except Exception as e:
    print("Encountered an error! Try again. \n")
    print(e)
    sys.exit(1)