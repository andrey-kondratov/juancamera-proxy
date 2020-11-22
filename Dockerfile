FROM arm32v7/alpine
RUN apk add build-base
WORKDIR /build
COPY src .
RUN make clean
RUN make
RUN chmod +x ./juanipc

FROM arm32v7/alpine
WORKDIR /app/
COPY --from=0 /build/juanipc .

CMD [ "./juanipc" ]
