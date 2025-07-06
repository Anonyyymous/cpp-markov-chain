import requests
from colorama import Fore, Style

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
                print(f"{test_no}/{test_size}: check {self.title}: {Fore.GREEN}PASSED{Style.RESET_ALL}")
                return True
            else:
                print(f"{test_no}/{test_size}: check '{self.title}': {Fore.RED}FAILED{Style.RESET_ALL}:\n'{result}'")
                return False
        except Exception as err:
            print(f"{test_no}/{test_size}: check {self.title}:\nAn error occured - {err}")
            return False


def equal(req, target):
    return req['response'] == target

def valid_equal(req, target):
    return req['response'] == target and req['valid'] == 1

def starts_with_one_of(str, prefixes):
    for prefix in prefixes:
        if str.startswith(prefix):
            return True
    return False

def successful_prompt(req):
    return req["valid"] == 1 and starts_with_one_of(req['response'], ["Tell him", "EAT SHIT"])


tests = [
    Test("", (lambda req: equal(req, "Model '' doesn't exist.")), "empty parameters"),
    Test("model=test-mdl", (lambda req: equal(req, "Model 'test-mdl' doesn't exist.")), "invalid model path"),
    Test("model=test-mdl.jkc", successful_prompt, "valid model path without prompt"),
    Test("model=test-mdl.jkc&prompt='Tell'", (lambda req: req['valid'] == 1 and req['response'].startswith("Tell him")), "single word prompt"),
    Test("model=test-mdl.jkc&prompt='EAT SHIT,'", (lambda req: valid_equal(req, "EAT SHIT, ASSHOLE! FALL OFF...")), "valid model path with prompt"),
    Test("model=test-mdl.jkc&prompt='Beep boop'", (lambda req: equal(req, "Beep boop") and req["valid"] == 0), "untrained prompt"),
    Test("model=test-mdl.jkc&prompt='EAT SHIT,'&soft_limit=a", (lambda req: equal(req, "couldnt parse hard/soft limit")), "testing invalid soft limit"),
    Test("model=test-mdl.jkc&prompt='EAT SHIT,'&hard_limit=a", (lambda req: equal(req, "couldnt parse hard/soft limit")), "testing invalid hard limit"),
    Test("model=test-mdl.jkc&prompt='EAT SHIT,'&soft_limit=2", (lambda req: valid_equal(req, "EAT SHIT, ASSHOLE!")), "testing valid soft limit"),
    Test("model=test-mdl.jkc&prompt='EAT SHIT,'&hard_limit=2", (lambda req: valid_equal(req, "EAT SHIT,...")), "testing valid hard limit"),
    Test("model=test-mdl.jkc&prompt='EAT SHIT,'&hard_limit=3&soft_limit=3", (lambda req: valid_equal(req, "EAT SHIT, ASSHOLE!")), "testing lower hard limit"),
    Test("model=test-mdl.jkc&prompt='Tell him to go'&hard_limit=100", (lambda req: valid_equal(req, "Tell him to go eat shit, Johnny.")), "long prompt")
]

failures = 0
for index, test in enumerate(tests):
    if not test.evaluate(index+1, len(tests)):
        failures += 1

print("tests complete -", failures, "failures")

exit(failures)