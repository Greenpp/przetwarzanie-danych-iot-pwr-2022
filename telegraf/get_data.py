import paho.mqtt.subscribe as subscribe
import datetime
import json
import fire

def get_data_points(device_name):
    m = subscribe.simple(
        topics=['#'],
        hostname="eu1.cloud.thethings.network",
        port=1883,
        auth={'username':"pwr-iot-2022",'password':'NNSXS.24TRHAQZJCF3UL74GMUXXVLR63KLYLKE2SEXSNI.O2GZUKUNG3I5DJSPGBKE7KCD45KSWQC45AGEVJ2DGCNJB3ZFYKWQ'},
        msg_count=2
        )
    if device_name == 'paw': 
        device = m[0]
    elif device_name == 'gil':
        device = m[1]
    else:
        raise Exception(f'No such device as {device_name}')
        
    payload = json.loads(device.payload)['uplink_message']['decoded_payload']
    humidity = payload['humidity']
    temperature = payload['temperature']
    timestamp = datetime.datetime.utcnow().isoformat()

    datapoints = [
        {
            'device_name': device_name,
            'time': timestamp,
            'fields':{
                'humidity': humidity,
                'temperature': temperature,
            }
        }
    ]
    return datapoints

if __name__ == '__main__':
    fire.Fire(get_data_points)
