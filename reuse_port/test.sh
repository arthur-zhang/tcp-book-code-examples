for i in {1..10} ; do
   echo "hello" | nc -i 1 localhost 9090
done
