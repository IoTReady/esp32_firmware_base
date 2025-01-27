## Running the example:

- You will need AWS configured in your device in order to automatically access your AWS and do the various steps above. If you haven't already:
    - Install the python AWS CLI on your machine
    ````
    $ pip3 install awscli
    ````
    - Once done, run the command below to configure your AWS security credentials by providing the respective values:
    ````
    $ aws configure

    AWS Access Key ID [None]:
    AWS Secret Access Key [None]:
    Default region name [None]:
    Default output format [None]:
    ````
    > For more details on AWS access keys :https://docs.aws.amazon.com/general/latest/gr/aws-sec-cred-types.html#access-keys-and-secret-access-keys<br>
    For more details on AWS CLI: https://docs.aws.amazon.com/cli/latest/userguide/cli-chap-welcome.html

- Create a python3 virtual environment and make sure it has pip ugraded.
````
$ python3 -m venv venv
$ source venv/bin/activate
$ python -m pip install -U pip
````
- You will need esptool installed. Just run:
````
$ pip install -r requirements.txt
````
- To ensure correct setup of ESP-IDF environment variables, make sure line 24 in prepare.sh points to your local ESP-IDF directory path. By default, it should be:
````
source ~/esp/esp-idf/export.sh
````
- Make sure you have configured the AWS variables in [registerDevice.py](./registerDevice.py#L19)
- Put your code project into a directory named `source`. This can be changed in the script.
- Connect your ESP32 
> This script is designed to be used for production firmware. Therefore, at every run, it stashes and pulls from the remote git repo. Please move ahead accordingly.
- The AWS certificates will be stored in a folder named `aws_credentials` directory according to the current setup. You can change this in the registerDevice.py file. Make sure the directory exists before you run the script.
- Make sure the prepare.sh file has executable permission:
````
$ sudo chmod a+x prepare.sh
````
- Run prepare.sh in your project folder.
````
$ ./prepare.sh
````

**Note:**
- If the project is not a git repository, comment the following lines in prepare.sh:
````
git stash
git pull
````

## SSL verification
For client authentication, AWS IoT uses X.509 certificates and private key. Client certificates must be registered with AWS IoT before a client can communicate with AWS IoT. AWS recommends that each device or client be given a unique certificate to enable fine-grained client management actions, including certificate revocation. For more visit [here](https://docs.aws.amazon.com/iot/latest/developerguide/x509-client-certs.html).

## Need for Simplicity

Here's the steps needed to prepare a device to be able to connect to AWS IoT servers:
1. Create a unique 'thing' on the AWS IoT server
2. Create a unique set of [certificates](https://docs.aws.amazon.com/iot/latest/developerguide/x509-client-certs.html) for the thing just created.
3. Create a AWS IoT Core [policy](https://docs.aws.amazon.com/iot/latest/developerguide/iot-policies.html) to control access to the AWS IoT Core server.
4. Attach the policy to the certificate used for the thing.
5. Upload these certificates into the device so that it can access them to authenticate the connection from the device to the AWS IoT server.

As you must have realized, this can be a tedious and time-consuming task, hence inefficient for preparing large number of devices. We wanted to create a tool that automates this process to make it a layman's task to prepare as many devices as possible in the shortest time possible while maintaining simplicity and configurability. The 'Prepare Script' takes care of all 5 steps mentioned above.


## The [Prepare Script](https://github.com/IoTReady/prepare_script_awsiot)
The prepare script is a tool that automates the creating and flashing of devices making them ready-to-deploy with just one command, saving time in abundance. It does the following:

1. Use esptool to get the default MAC address of the device.
2. Creates an AWS policy if it does not already exist. To learn about policies in AWS, visit [here](https://docs.aws.amazon.com/iot/latest/developerguide/iot-policies.html).
3. Create keys(private and public) and the certificate for the device and saves them as files.
4. Attach the existing/created policy in step 2 to the certificate.
5. Create a new thing with the MAC address obtained in step1 as thing name.
6. Copy/embed the downloaded certificate and keys files into the necessary folder.

![prepare_script_flow](/doc/assets/prepare_script_flow.png)

## SPIFFS Handling

Along with the AWS IoT perks, this example also handles the SPIFFS integration. The unique certificates that are downloaded for each device are stored into a directory, which is then created into an image and stored into the SPIFFS partition space in the ESP32 (using the [spiffsgen.py](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/storage/spiffs.html#spiffsgen-py) tool). The code then has provisions to read these files and use them for SSL verification and authentication.
