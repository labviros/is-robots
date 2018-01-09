FROM git.is:5000/is-cpp:1-aria

COPY . /build
RUN cd /build \
 && make \
 && mkdir -p lib \
 && for lib in `qemu-arm -E LD_TRACE_LOADED_OBJECTS=1 robot-gateway | awk 'BEGIN{ORS=" "}$1~/^\//{print $1}$3~/^\//{print $3}' | sed 's/,$/\n/'`; do cp $lib lib/; done 