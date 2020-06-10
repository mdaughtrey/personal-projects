#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>

extern "C"
{
#include <usb.h>
#include <unistd.h>
#include <errno.h>
}

#include <usbdriver.h>

using namespace std;
#define USB_TIMEOUT 500

usb_dev_handle * hTinyISP;

void dataToDisplay(string data, int pauseTime, string pauseToken);
void dataToDisplay(string data);

usb_dev_handle * acquireTinyISP(void)
{
    struct usb_bus * thisBus;
    struct usb_device * thisDevice = NULL;

    usb_init();
    usb_find_busses();
    usb_find_devices();

    for (int retry = 0; retry < 1 && NULL == thisDevice; retry++)
    {

        for (thisBus = usb_get_busses(); NULL != thisBus; thisBus = thisBus->next)
        {
	  
            for (thisDevice = thisBus->devices; NULL != thisDevice; thisDevice = thisDevice->next)
            {
                usb_dev_handle * device;
                device = usb_open(thisDevice);
                cout << "Vendor " << hex << setw(4) << setfill('0')
                     << thisDevice->descriptor.idVendor
                     << " Product " << thisDevice->descriptor.idProduct
                     << endl;
  

                if (TINYISP_VENDORID == thisDevice->descriptor.idVendor &&
                    TINYISP_PRODUCTID == thisDevice->descriptor.idProduct)
                {
                    cout << "Acquired device" << endl;
                    return device;
                }
                usb_close(device);
	      
            }
        }
        cerr << "No device found, sleeping..." << endl;
        sleep(1);
    }
}

void usb_to ( int req, int val, int size )
{
    int response;
    int rc;
    cerr << "Command " << hex << setw(2) << setfill('0') << (val & 0xff);
    if ((val & 0xff) > 0x20 && (val & 0xff) < 0x7e)
    {
        std::cerr << "(" 
                  <<static_cast<char>(val & 0xff)
                  << ")";
    }

    rc =  usb_control_msg( hTinyISP,
                           USB_ENDPOINT_IN | USB_TYPE_VENDOR | USB_RECIP_DEVICE,
                           req, val, size, reinterpret_cast<char *>(&response), 4, USB_TIMEOUT );
    //		   req, val, size, NULL, 0, USB_TIMEOUT );
    cerr << " Returns " << rc;
    if (rc)
    {
        cerr << " Response: " 
             << std::hex << std::setw(2) << setfill('0') << (response & 0xff);
        if ((response & 0xff) > 0x20 && (response & 0xff) < 0x7e)
        {
            std::cerr << "(" 
                      <<static_cast<char>(response & 0xff)
                      << ")";
        }
    }
    cerr << std::endl;
}

void dataToDisplay(string data, int pauseTime, string pauseToken)
{
    int tokenIndex(0);

    while (0 != data.length())
    {
        tokenIndex = data.find(pauseToken, tokenIndex);
        if (string::npos != tokenIndex)
        {
            dataToDisplay(std::string(data.begin(), data.begin() + tokenIndex));
            usleep(pauseTime * 1000);
            data.erase(data.begin(), data.begin() + tokenIndex + pauseToken.length());
        }
        else
        {
            dataToDisplay(data);
            data.clear();
        }
    }

}

void dataToDisplay(string data)
{
    cout << "Display " << data.c_str() << endl;
  
    usb_to(USBTINY_CLR, 1, 1); // /SS low
    for (string::iterator iData = data.begin();
         iData != data.end();
         iData++)
    {
        usb_to(USBTINY_SPI1, *iData,  1);	// data
    }
    usb_to(USBTINY_SET, 1, 1); // /SS high
}

int main(int argc, char ** argv)
{
    bool interactive(false);
    bool loop(false);
    int pauseTime(0);
    string pauseToken;
    string fifoName;
    ifstream fifoIn;
    ofstream fifoOut;
    int argIndex = 1;
    while (argIndex < argc)
    {
        if ('-' == argv[argIndex][0])
        {
            switch (argv[argIndex][1])
            {
            case 'i':
                interactive = true;
                argIndex++;
                break;

            case 'l': 
                loop = true; 
                argIndex++;
                break;

            case 'p':
            {
                argIndex++;
                stringstream arg(argv[argIndex++]);
                arg >> pauseTime;
                break;
            }
	      
            case 'P':
                argIndex++;
                pauseToken = argv[argIndex++];
                break;

            case 'f':
                argIndex++;
                fifoName = argv[argIndex++];
                break;
            }
        }
        else
            break;
    }


    //  struct usb_bus * busses;
    hTinyISP = NULL;

    for (int retry = 4; retry && ! hTinyISP; retry--)
    {
      
        hTinyISP = acquireTinyISP();
    }

    if (NULL == hTinyISP)
    {
        cerr << "Could not acquire device" << endl;
        exit(1);
    }

    usb_to( USBTINY_POWERUP, USB_TIMEOUT, RESET_HIGH );
    usleep(500000);
    usb_to( USBTINY_POWERUP, USB_TIMEOUT, RESET_LOW );
    usleep(500000);
    usb_to( USBTINY_POWERUP, USB_TIMEOUT, RESET_HIGH );
    usleep(500000);

    //
    // Set PB1 to output for ~Slave Select
    // 1011 0010
    //
    usb_to(USBTINY_DDRWRITE, 0xb2, 1);

    usb_to(USBTINY_SET, 1, 1); // /SS high
    do
    {
        if (0 != pauseTime)
        {
            dataToDisplay(argv[argIndex], pauseTime, pauseToken);
        }
        else
        {
            string buffer;
            if (true == interactive)
            {
                buffer.clear();
                cout << "Interactive (.. to exit):" << std::endl;
                cin >> buffer;
                if (".." == buffer)
                {
                    break;
                }
                dataToDisplay(buffer);
                continue;
            }
            if (fifoName.length())
            {
                cout << "Opening " << fifoName << endl;
                std::string fifoName2(fifoName + ".in");
                fifoIn.open(fifoName2.c_str());
                fifoName2 = fifoName + ".out";
                fifoOut.open(fifoName2.c_str());
                if (!fifoIn || !fifoOut)
                {
                    cerr << "Error opening " << fifoName
                         << ": errno " << errno
                         << std::endl;
                    exit(1);
                }

                while (getline(fifoIn, buffer))
                {
                    fifoOut << buffer << endl;
                    dataToDisplay(buffer);
                }
                fifoIn.close();
                fifoOut.close();
                cout << "end getline loop" << endl;
            }
            else
            {
                dataToDisplay(argv[argIndex]);
            }
        }
        cout << "Looping " << loop << endl;
    }  while (true == loop);


    usb_to( USBTINY_POWERDOWN, 0, 0 );
    usb_close(hTinyISP);
  
}
