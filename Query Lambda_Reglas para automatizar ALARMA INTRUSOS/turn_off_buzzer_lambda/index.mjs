import AWS from 'aws-sdk';

const IotData = new AWS.IotData({
  endpoint: 'a2gyx6o3r72s2g-ats.iot.us-east-1.amazonaws.com'
});

function getShadowPromise(params) {
  return new Promise((resolve, reject) => {
      IotData.getThingShadow(params, (err, data) => {
          if (err) {
              console.log(err, err.stack);
              reject('Failed to get thing shadow ${err.errorMessage}');
          } else {
              resolve(JSON.parse(data.payload));
          }
      });
  });
}

export const handler = async (event) => {
    console.log('Evento recibido:', JSON.stringify(event));

    const thingName = event.thing;
    const alarmEvent = event.message;
    
    const ShadowParams = {
      thingName: thingName
    };

    if (alarmEvent === 1 || alarmEvent === 0) {
      let buzzerOn;
      let intruderDetected;
        await getShadowPromise(ShadowParams).then((result) => buzzerOn = result.state.reported.buzzer_on);
        await getShadowPromise(ShadowParams).then((result) => intruderDetected = result.state.reported.intruder_detected);
        console.log("Buzzer State: ", buzzerOn)
        console.log("Detection State: ", intruderDetected)
        if(buzzerOn === 1 && intruderDetected != 1) {
          try {
              const payload = {
                state: {
                    desired: {
                        buzzer_on: 0
                    }
                }
              };
              
              const params = {
                  thingName: thingName,
                  payload: JSON.stringify(payload)
              };
              const result = await IotData.updateThingShadow(params).promise();
              console.log('Shadow actualizado:', result);
          } catch (err) {
              console.error('Error al actualizar el shadow:', err);
              throw new Error('No se pudo actualizar el shadow.');
          }
        }
    }

    return {
        statusCode: 200,
        body: JSON.stringify('Shadow actualizado con éxito'),
    };
};
