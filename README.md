# cpp-markov-chain
A simple C++ markov chain and HTTP server, designed to be used in tandem.
## The Chain Terminal
This is the main method of interacting with chains locally, and can be used to load, create and run chains.
## About Chains
Chains are implemented via C++ maps, and when one is queried, the query includes a hard/soft limit.
Chains work on spaces - they include punctuation in words (e.g. 'complete.'). The soft limit determines the minimum word count the chain can run for before a full stop ends the output (this does not include instances where the chain runs out of data to spit out), such as 'My name is Yoshikage Kira.', where a soft limit of 2 would lead to the output ending here.
Similarly, the hard limit is an absolute cut-off point - once the hard limit is reach the ouput immediately ends (e.g. 'My name is' with a hard limit of 3).

The number of words a chain has in its context is referred to as its 'length'. A chain with a length of 1 just knows what word follows another, a chain with a length of 2 knows that 'I am' follows 'a' and 'am a' follows 'fish', in the sentence 'I am a fish'.
### key Characters
- 'n' create a new chain

Directories are separated from the command character by a single space

- 'l' loads a chain from a given directory*
- 't' trains a chain on a given directory
- 's' saves a chain to a given directory*
- 'c' changes a chain option (e.g. default soft/hard limit)
- - 's' for soft limit
- - 'h' for hard limit
- '>' regurgitates the rest of the input

- 'q' quits the program

Otherwise, it is assumed that the text was meant to be as input to the chain for regurgitation.

\* chains should be saved with an .jkc extension. If the chain has already been saved/loaded, it will remember this location, so you won't need to specify the location later
## Running The Server
When initialising the server, do ./run-server (-c [server config path]) (q).
The server config is optional and stores settings about the server. If a config cannot be found, values will default to:
> port = 6678
> model-directory = ../models/

'q' will run the server in quiet mode, with less info being printed to the terminal
### Running The Server as a Container
The container uses a two-stage build process (hence why .dockerignore is a bit bland) to create only the necessary files in the image (the server executable, config file (TODO), and models directory).
## Querying The Server
The http request contains up to 4 parameters - the model, the prompt, the hard limit, and the soft-limit.
An example request would be 'http://localhost:6678/model=mdl1.jkc&prompt=My name is&soft_limit=3'. Note that 'hard_limit' is not included, so will default to whatever value is inside the server's config file.
In this prompt, the model queried would be (server.model-directory + '/mdl1.jkc').
## Testing The Chain
The chain test suite first trains a model on a very small input, and tests it locally, including checking for random chance (one input phrase having multiple output words).
Next, similar tests are done via the API through python and the requests package.