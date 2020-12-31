NCAT=/usr/bin/ncat

if [ $# -ne 2 ]; then
    echo "Usage: $(basename $0) sensor_name port"
    exit 1
fi

sensor_name="$1"
port="$2"

function get_metrics() {
    #IFS=, read temp humid <<< "$(sudo ./dht 2>/dev/null)"
    IFS=, read temp humid <<< "$(sudo ./dht)"
    echo "dht_sensor_temp{sensor=\"$sensor\"} $temp"
    echo "dht_sensor_humidity{sensor=\"$sensor\"} $humid"
}

while true; do 
    echo "netcat listening at $(date)"
    $NCAT -l -p $port -w 1 -c "./dht_wrapper.sh $sensor_name"
done
