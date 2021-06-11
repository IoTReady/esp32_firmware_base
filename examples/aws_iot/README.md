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

- You will need esptool and boto3 installed. Just run:
````
$ pip3 install -r requirements.txt
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



## The [Prepare Script](https://github.com/IoTReady/prepare_script_awsiot)
The prepare script is a tool that automates the creating and flashing of devices making them ready-to-deploy with just one command, saving time in abundance. It does the following:

1. Use esptool to get the default MAC address of the device.
2. Creates an AWS policy if it does not already exist. To learn about policies in AWS, visit [here](https://docs.aws.amazon.com/iot/latest/developerguide/iot-policies.html).
3. Create keys(private and public) and the certificate for the device and saves them as files.
4. Attach the existing/created policy in step 2 to the certificate.
5. Create a new thing with the MAC address obtained in step1 as thing name.
6. Copy/embed the downloaded certificate and keys files into the necessary folder.

![prepare_script_flow](/doc/assets/prepare_script_flow.png)

