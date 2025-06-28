import requests
from functools import partial

port = 6678
addr = "127.0.0.1"

class Test:
    def __init__(self, arguement, func, title):
        self.arguement = f"http://{addr}:{port}/?" + arguement
        self.func = func
        self.title = title

    def evaluate(self, test_no, test_size):
        try:
            result = requests.get(self.arguement).json()
            if(self.func(result)):
                print(f"{test_no}/{test_size}: check {self.title}: PASSED")
                return True
            else:
                print(f"{test_no}/{test_size}: check '{self.title}': FAILED:\n'{result}'")
                return False
        except Exception as e:
            print(f"{test_no}/{test_size}: check {self.title}:\nAn error occured - {e}")
            return False
        
def status_equals(req, target_status, target_response):
    return req['response'] == target_response and req['status'] == target_status
    
def call_was_successful(req):
    return req['status'] == 200

def equal(req, target):
    return req['response'] == target

def starts_with_one_of(str, prefixes):
    for prefix in prefixes:
        if str.startswith(prefix):
            return True
    return False

tests = [
    #Test("model=test.jkc&prompt='EAT SHIT'", {'status': 200, 'response': "Model 'test' doesn't exist"}, "valid model path without prompt"),
    #Test("", partial(status_equals, target_status=404, response="Model '' doesn't exist"), "empty parameters"),
    Test("", (lambda req: req == {'status': 404, 'response': "Model '' doesn't exist"}), "empty parameters"),
    Test("model=test-mdl", (lambda req: req == {'status': 404, 'response': "Model 'test-mdl' doesn't exist"}), "invalid model path"),
    Test("model=test-mdl.jkc", call_was_successful, "valid model path without prompt"),
    Test("model=test-mdl.jkc&prompt='EAT SHIT,'", (lambda req: equal(req, "EAT SHIT, ASSHOLE! FALL OFF...")), "valid model path with prompt"),
    Test("model=test-mdl.jkc&prompt='Beep boop'", (lambda req: starts_with_one_of(req['response'], ["Beep boop Tell him", "Beep boop EAT SHIT"])), "untrained prompt"),
    Test("model=test-mdl.jkc&prompt='EAT SHIT,'&soft_limit=a", (lambda req: req == {'status': 400, 'response': "couldnt parse hard/soft limit"}), "testing invalid soft limit"),
    Test("model=test-mdl.jkc&prompt='EAT SHIT,'&hard_limit=a", (lambda req: req == {'status': 400, 'response': "couldnt parse hard/soft limit"}), "testing invalid hard limit"),
    Test("model=test-mdl.jkc&prompt='EAT SHIT,'&soft_limit=2", (lambda req: equal(req, "EAT SHIT, ASSHOLE!")), "testing valid soft limit"),
    Test("model=test-mdl.jkc&prompt='EAT SHIT,'&hard_limit=2", (lambda req: equal(req, "EAT SHIT,...")), "testing valid hard limit"),
    Test("model=test-mdl.jkc&prompt='EAT SHIT,'&hard_limit=3&soft_limit=3", (lambda req: equal(req, "EAT SHIT, ASSHOLE!")), "testing lower hard limit")
]

failures = 0
for index, test in enumerate(tests):
    if not test.evaluate(index+1, len(tests)):
        failures += 1

print("tests complete -", failures, "failures")

if failures != 0:
    exit(-1)