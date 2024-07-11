# Phishkiller

A C remake for [Phyton Phishkiller by CybrZone](https://github.com/CybrZone/phishkiller/)                        

## Usage
1. Clone the git repo:
```bash
git clone https://github.com/erukavina/phishkiller.git
```

2. Change the current working dir to phishkiller:
```bash
cd phishkiller
```

3. Install required dependencies:
```bash
sudo apt-get update
sudo apt-get install build-essential
sudo apt-get install libcurl4-openssl-dev
```

4. Compile the program:        
```bash 
gcc -o phishkiller main.c -lcurl -lpthread

```

5. Run the program:
```bash 
sudo ./phishkiller
```
The program will prompt you to enter the URL of the target you want to flood:    
```bash
Enter the URL of the target you want to flood:
```

Enter the URL and press Enter. The program will start sending HTTP POST requests to the specified URL using multiple threads.

---

**Note**: This tool should be used responsibly and only for educational purposes or with permission from the target server. Unauthorized use of such tools can be illegal and unethical.

