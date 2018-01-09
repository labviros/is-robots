FROM viros/is-cpp:1-armv6

RUN wget http://robots.mobilerobots.com/ARIA/download/current/ARIA-src-2.9.1.tar.gz \
 && tar -xf ARIA-src-2.9.1.tar.gz \
 && cd Aria-src-2.9.1 \
 && make allLibs -j3 \
 && cp lib/* /usr/arm-linux-gnueabihf/lib \
 && mkdir /usr/arm-linux-gnueabihf/include/Aria \
 && cp include/* /usr/arm-linux-gnueabihf/include/Aria \
 && cd .. \
 && rm -f ARIA-src-2.9.1.tar.gz \
 && rm -rf Aria-src-2.9.1