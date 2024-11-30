//turn_on_buzzer_function

import AWS from 'aws-sdk';

const IotData = new AWS.IotData({
  endpoint: 'a2gyx6o3r72s2g-ats.iot.us-east-1.amazonaws.com'
});

export const handler = async (event) => {
    console.log('Evento recibido:', JSON.stringify(event));

    const thingName = event.thing;
    const intruderDetected = event.message;
    
    if (intruderDetected === 1) {
        const payload = {
            state: {
                desired: {
                    buzzer_on: 1
                }
            }
        };
        
        const params = {
            thingName: thingName,
            payload: JSON.stringify(payload)
        };

        try {
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
