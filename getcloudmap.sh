#!/usr/local/bin/zsh

FIL=http://www.fourmilab.ch/cgi-bin/uncgi/Earth\?imgsize=640\&opt=-p\&lat=-21\&ns=North\&lon=-134\&ew=West\&alt=986\&img=irsat.bmp\&daynight=-d
SLEEP=5

while echo "downloading $FIL..."
do
if wget --cache=off --continue "$FIL" --output-document=clouds.html
then echo returned OK; break
else echo returned false
echo "sleeping for $SLEEP seconds..."
sleep $SLEEP
fi
done

grep "/earthview/cache/[0-9]*.gif" clouds.html | gawk -F"\"" '{ print "http://www.fourmilab.ch"$2
}' | read x

while echo "getting image..."
do
if wget --cache=off --continue "$x" --output-document=clouds.gif
then echo returned OK; break
else echo returned false
echo "sleeping for $SLEEP seconds..."
sleep $SLEEP
fi
done

gif2png clouds.gif

