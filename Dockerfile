# the point of this is making/running one environment to build the project
# so that it can run in the actual one, without every single file
FROM alpine AS build-env

RUN apk add --no-cache build-base cmake

WORKDIR /app

COPY . .

# make the makefile
RUN cmake -B build -S /app/build/
# compile everything 
RUN cmake --build /app/build/

# the final image
FROM alpine:latest

# install dependencies
RUN apk add --no-cache libstdc++ libgcc

# should also copy server config over
COPY --from=build-env /app/build/run-server /app/run-server
COPY --from=build-env /app/models /models

WORKDIR /app

CMD ["/app/run-server"]