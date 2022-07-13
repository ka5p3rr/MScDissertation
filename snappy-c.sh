set -x
cd snappy-c
unzip compression-corpus/enwik/enwik8.zip -d compression-corpus/enwik/
gmake
gmake test