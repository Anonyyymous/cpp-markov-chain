import requests

port = 6678
addr = "127.0.0.1"

class Test:
    def __init__(self, arguement, func, title):
        self.arguement = f"http://{addr}:{port}/shart/pass/?" + arguement
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

tests = [
    #Test("model=test.jkc&prompt='EAT SHIT'", {'status': 200, 'response': "Model 'test' doesn't exist"}, "valid model path without prompt"),
    Test("", (lambda req: req == {'status': 404, 'response': "Model '' doesn't exist"}), "empty parameters"),
    Test("model=test-mdl", (lambda req: req == {'status': 404, 'response': "Model 'test' doesn't exist"}), "invalid model path"),
    Test("model=test-mdl.jkc", (lambda req: req['status'] == 200), "valid model path without prompt"),
    Test("model=test-mdl.jkc&prompt='EAT SHIT'", (lambda req: req['status'] == 200), "valid model path without prompt")
]

""" ,
    Test("model=test", "", "invalid model path"),
    Test("model=test.jkc", "", "valid model path without prompt"),
    Test("model=test.jkc&prompt=to go", "", "valid model path with valid prompt"),
    Test("model=test.jkc&prompt=", "", "invalid prompt"),
    Test("model=test.jkc&prompt=Beep boop", "", "untrained prompt"),
    Test("model=test.jkc&prompt=to go", "", "soft limit"),
    Test("model=test.jkc&prompt=to go", "", "hard limit"),
    Test("model=test.jkc&prompt=to go", "", "hard/soft limit"),
    Test("model=test.jkc&prompt=Tell him", "", "hard/soft limit") """

for index, test in enumerate(tests):
    if not test.evaluate(index+1, len(tests)):
        #exit(-1)
        print()

