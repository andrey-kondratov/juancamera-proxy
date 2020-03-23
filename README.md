# Guangzhou Juan IP Camera proxy

A proxy server app for streaming video from a Guangzhou Juan IP camera without
its custom handshaking.

## Usage

The application listens on port 64444 and responds with the raw video stream
to incoming connections.

```
./juanipc <hostname>
```

where `hostname` - the hostname or IP address of the camera.

## Limitations

Only one client connection is supported.

The camera must have a default `admin` user with empty password, see
[this gist](https://gist.github.com/maxious/c8915a436b532ab09e61bf937295a5d2)
for details.

## Build

Go to src/ and run `make`.
