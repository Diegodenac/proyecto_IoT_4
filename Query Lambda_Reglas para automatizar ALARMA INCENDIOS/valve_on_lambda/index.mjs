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
    let valveState;

    const thingName = event.thing;

    const ShadowParams = {
      thingName: thingName
    };
    
    await getShadowPromise(ShadowParams).then((result) => valveState = result.state.reported.valve_state);
    if(valveState === 0) {
      try {
          const payload = {
            state: {
                desired: {
                    valve_state: 1
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

    return {
        statusCode: 200,
        body: JSON.stringify('Shadow actualizado con éxito'),
    };
};
