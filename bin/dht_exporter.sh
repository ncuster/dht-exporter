NCAT=/usr/bin/ncat

if [ $# -ne 1 ]; then
    echo "Usage: $(basename $0) sensor_name"
    exit 1
fi

sensor_name="$1"

function get_metrics() {
    #IFS=, read temp humid <<< "$(sudo ./dht 2>/dev/null)"
    IFS=, read temp humid <<< "$(sudo ./dht)"
    echo "dht_sensor_temp{sensor=\"$sensor\"} $temp"
    echo "dht_sensor_humidity{sensor=\"$sensor\"} $humid"
}

while true; do 
    $NCAT -l -p 8005 -w 1 -c "sudo ./dht $sensor_name"
done
