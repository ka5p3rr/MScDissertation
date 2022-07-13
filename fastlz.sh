set -x
cd FastLZ
git clone git@github.com:ariya/compression-corpus.git
unzip compression-corpus/enwik/enwik8.zip -d compression-corpus/enwik/
cd tests
gmake