import requests

class Test:
    def __init__(self, arguement, expected, title):
        self.arguement = arguement
        self.expected = expected
        self.title = title

    def evaluate(self):
        result = requests.get(self.arguement).json()
        if(result == self.expected):
            print(f"check {self.title}: PASSED")
        else:
            print(f"check {self.title}: FAILED,", result)

tests = [
    Test("localhost:6678/", "", "empty parameters"),
    Test("localhost:6678/model=", "", "empty model"),
    Test("localhost:6678/model=test", "", "invalid model path"),
    Test("localhost:6678/model=test.jkc", "", "valid model path without prompt"),
    Test("localhost:6678/model=test.jkc&prompt=to go", "", "valid model path with valid prompt"),
    Test("localhost:6678/model=test.jkc&prompt=", "", "invalid prompt"),
    Test("localhost:6678/model=test.jkc&prompt=Beep boop", "", "untrained prompt"),
    Test("localhost:6678/model=test.jkc&prompt=to go", "", "soft limit"),
    Test("localhost:6678/model=test.jkc&prompt=to go", "", "hard limit"),
    Test("localhost:6678/model=test.jkc&prompt=to go", "", "hard/soft limit"),
    Test("localhost:6678/model=test.jkc&prompt=Tell him", "", "hard/soft limit")
]

[test.evaluate() for test in tests]

