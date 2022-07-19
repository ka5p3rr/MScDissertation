set -x
cd FastLZ

FOLDER="compression-corpus"
URL="git@github.com:ariya/compression-corpus.git"

if [ ! -d "$FOLDER" ] ; then
    git clone $URL $FOLDER
    unzip compression-corpus/enwik/enwik8.zip -d compression-corpus/enwik/
else
    cd "$FOLDER"
    git pull $URL
    cd ..
fi

cd tests
gmake