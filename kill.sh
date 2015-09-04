a=$(ps -aux | grep "iperf3" | awk '{print $2}')
echo $a
for i in $a;
	do
		sudo kill -9 $i
	done	

