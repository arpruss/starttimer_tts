python voice.py "Please Wait" PleaseWait.wav
python voice.py "25" n25.wav
python voice.py "20" n20.wav
python voice.py "15" n15.wav
python voice.py "10" n10.wav
python voice.py "5" n5.wav
python voice.py "4" n4.wav
python voice.py "3" n3.wav
python voice.py "2" n2.wav
python voice.py "1" n1.wav
python voice.py "Ready!" Ready.wav
python voice.py "Canceled!" Canceled.wav
rm samples.h
for x in PleaseWait n25 n20 n15 n10 n5 n4 n3 n2 n1 Ready Canceled ; do 
    ffmpeg -y -i $x.wav -f u8 -acodec pcm_u8 -ac 1 -ar 8000 $x.u8
    rm $x.wav
    python trim80.py $x.u8
    echo "static const" >> samples.h
    xxd -i $x.u8 >> samples.h
done
