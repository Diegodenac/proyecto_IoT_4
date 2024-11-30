import * as Alexa from 'ask-sdk-core';
import AWS from 'aws-sdk';
import { DynamoDBClient } from '@aws-sdk/client-dynamodb';
import { GetCommand, DynamoDBDocumentClient } from '@aws-sdk/lib-dynamodb';

const IotData = new AWS.IotData({ endpoint: 'a2gyx6o3r72s2g-ats.iot.us-east-1.amazonaws.com' });


const client = new DynamoDBClient({});
const docClient = DynamoDBDocumentClient.from(client);

const DisarmAlarmIntruderParams = {
    thingName: '',
    payload: '{"state": {"desired": {"alarm_armed": 0}}}'
};

const ArmAlarmIntruderParams = {
    thingName: '',
    payload: '{"state": {"desired": {"alarm_armed": 1}}}'
};

const TurnOffBuzzerParams = {
    thingName: '',
    payload: '{"state": {"desired": {"buzzer_on": 0}}}'
};

const TurnOnBuzzerParams = {
    thingName: '',
    payload: '{"state": {"desired": {"buzzer_on": 1}}}'
};

const ShadowIntruderParams = {
    thingName: ''
};

function setAlarmIntruderThing(thing){
    DisarmAlarmIntruderParams.thingName = thing;
    ArmAlarmIntruderParams.thingName = thing;
    TurnOffBuzzerParams.thingName = thing;
    TurnOnBuzzerParams.thingName = thing;
    ShadowIntruderParams.thingName = thing;
};

const OpenValveParams = {
    thingName: '',
    payload: '{"state": {"desired": {"valve_state": 1}}}'
};

const CloseValveParams = {
    thingName: '',
    payload: '{"state": {"desired": {"valve_state": 0}}}'
};

const ShadowFireParams = {
    thingName: ''
};

function setAlarmFireThing(thing){
    OpenValveParams.thingName = thing;
    CloseValveParams.thingName = thing;
    ShadowFireParams.thingName = thing;
};

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

const LaunchRequestHandler = {
    canHandle(handlerInput) {
        return Alexa.getRequestType(handlerInput.requestEnvelope) === 'LaunchRequest';
    },
    async handle(handlerInput) {

        const speakOutput = 'Bienvenido a tu hogar seguro. Para comenzar, por favor dime tu nombre de usuario.';

        return handlerInput.responseBuilder
            .speak(speakOutput)
            .reprompt(speakOutput)
            .getResponse();
    }
};

const UserValidationIntentHandler = {
    canHandle(handlerInput) {
        return Alexa.getRequestType(handlerInput.requestEnvelope) === 'IntentRequest' &&
               Alexa.getIntentName(handlerInput.requestEnvelope) === 'UserValidationIntent';
    },
    async handle(handlerInput) {
        const username = Alexa.getSlotValue(handlerInput.requestEnvelope, 'username');

        if (!username) {
            const speakOutput = 'Por favor, proporciona tu nombre de usuario para continuar.';
            return handlerInput.responseBuilder
                .speak(speakOutput)
                .reprompt(speakOutput)
                .getResponse();
        }

        const params = {
            TableName: 'user-thing',
            Key: {
              serial: 0,
              username: username,
            },
        };

        try {
            const data = await docClient.send(new GetCommand(params));
            
            if (!data.Item) {
                const speakOutput = 'No hemos encontrado un usuario con ese nombre. Por favor, intenta de nuevo.';
                setAlarmIntruderThing('');
                setAlarmFireThing('');
                return handlerInput.responseBuilder
                    .speak(speakOutput)
                    .reprompt(speakOutput)
                    .getResponse();
            } else {
                const speakOutput = `¡Bienvenido ${username}! Puedes armar y desarmar tu alarma, encender y apagar la bocina, abrir y cerrar la valvula, consultar si existen intrusos y la calidad del aire. ¿Qué deseas hacer?`;
                setAlarmIntruderThing(data.Item.thing_alarm_intruder);
                setAlarmFireThing(data.Item.thing_alarm_fire);
                return handlerInput.responseBuilder
                    .speak(speakOutput)
                    .reprompt(speakOutput)
                    .getResponse();
            }
        } catch (error) {
            console.error('Error al consultar DynamoDB:', error);
            const speakOutput = `${username}, hubo un problema al verificar tu usuario. Intenta de nuevo más tarde.`;
            return handlerInput.responseBuilder
                .speak(speakOutput)
                .reprompt(speakOutput)
                .getResponse();
        }
    }
};

const ArmAlarmIntentHandler = {
    canHandle(handlerInput) {
        return Alexa.getRequestType(handlerInput.requestEnvelope) === 'IntentRequest'
            && Alexa.getIntentName(handlerInput.requestEnvelope) === 'ArmAlarmIntent';
    },
    handle(handlerInput) {
        var speakOutput = "Debes iniciar sesion con un usuario para continuar";
        try{
            IotData.updateThingShadow(ArmAlarmIntruderParams, function(err, data) {
                if (err) console.log(err);
            });
            speakOutput = 'Solicitaste armar la alarma';
        } catch(error){
            console.error('Error al publicar en shadow:', error);
        }
        return handlerInput.responseBuilder
            .speak(speakOutput)
            .reprompt(speakOutput)
            .getResponse();
    }
};

const DisarmAlarmIntentHandler = {
    canHandle(handlerInput) {
        return Alexa.getRequestType(handlerInput.requestEnvelope) === 'IntentRequest'
            && Alexa.getIntentName(handlerInput.requestEnvelope) === 'DisarmAlarmIntent';
    },
    handle(handlerInput) {
        var speakOutput = "Debes iniciar sesion con un usuario para continuar";
        try{
            IotData.updateThingShadow(DisarmAlarmIntruderParams, function(err, data) {
                if (err) console.log(err);
            });
            speakOutput = 'Solicitaste desarmar la alarma';
        } catch(error){
            console.error('Error al publicar en shadow:', error);
        }
        return handlerInput.responseBuilder
            .speak(speakOutput)
            .reprompt(speakOutput)
            .getResponse();
    }
};

const DangerIntentHandler = {
    canHandle(handlerInput) {
        return Alexa.getRequestType(handlerInput.requestEnvelope) === 'IntentRequest'
            && Alexa.getIntentName(handlerInput.requestEnvelope) === 'DangerIntent';
    },
    handle(handlerInput) {
        var speakOutput = "Debes iniciar sesion con un usuario para continuar";
        try{
            IotData.updateThingShadow(TurnOnBuzzerParams, function(err, data) {
                if (err) console.log(err);
            });
            speakOutput = 'Estas en peligro, encendiendo la bocina';
        } catch(error){
            console.error('Error al publicar en shadow:', error);
        }
        return handlerInput.responseBuilder
            .speak(speakOutput)
            .reprompt(speakOutput)
            .getResponse();
    }
};

const SafeIntentHandler = {
    canHandle(handlerInput) {
        console.log("RecuperaIntent");
        return Alexa.getRequestType(handlerInput.requestEnvelope) === 'IntentRequest'
            && Alexa.getIntentName(handlerInput.requestEnvelope) === 'SafeIntent';
    },
    async handle(handlerInput) {
        let intruderDetected;
        var speakOutput = "Debes iniciar sesion con un usuario para continuar";
        try{
            await getShadowPromise(ShadowIntruderParams).then((result) => intruderDetected = result.state.reported.intruder_detected);
        }catch{
            console.error('Error al consultar en shadow:', error)
        }
        if(!intruderDetected){
            try{
                IotData.updateThingShadow(TurnOffBuzzerParams, function(err, data) {
                    if (err) console.log(err);
                });
                speakOutput = 'Estas chill de cajones, apagando la bocina';
            }catch{
                console.error('Error al publicar en shadow:', error);
            }
        }
        else{
            speakOutput = 'No se puede desactivar la bocina cuando hay intrusos';
        }
        return handlerInput.responseBuilder
            .speak(speakOutput)
            .reprompt(speakOutput)
            .getResponse();
    }
};

const VerifyIntruderIntentHandler = {
    canHandle(handlerInput) {
        return Alexa.getRequestType(handlerInput.requestEnvelope) === 'IntentRequest'
            && Alexa.getIntentName(handlerInput.requestEnvelope) === 'VerifyIntruderIntent';
    },
    async handle(handlerInput) {
        let intruderDetected;
        var speakOutput = "Debes iniciar sesion con un usuario para continuar";
        try{
            await getShadowPromise(ShadowIntruderParams).then((result) => intruderDetected = result.state.reported.intruder_detected);
        }catch{
            console.error('Error al consultar en shadow:', error)
        }
        if (intruderDetected == 0) {
            speakOutput = 'No se detectaron intrusos, estas a salvo';
        } else if (intruderDetected == 1) {
            speakOutput = 'Se detectó la presencia de intrusos, huyamos!';
        } else {
            speakOutput = 'No se pudo consultar si existen intrusos, por favor intente más tarde';
        }
        return handlerInput.responseBuilder
            .speak(speakOutput)
            .reprompt(speakOutput)
            .getResponse();
    }
};

const OpenValveIntentHandler = {
    canHandle(handlerInput) {
        return Alexa.getRequestType(handlerInput.requestEnvelope) === 'IntentRequest'
            && Alexa.getIntentName(handlerInput.requestEnvelope) === 'OpenValveIntent';
    },
    handle(handlerInput) {
        var speakOutput = "Debes iniciar sesion con un usuario para continuar";
        try{
            IotData.updateThingShadow(OpenValveParams, function(err, data) {
                if (err) console.log(err);
            });
            speakOutput = 'Abriendo la válvula, lluvia! wiii';
        } catch(error){
            console.error('Error al publicar en shadow:', error);
        }
        return handlerInput.responseBuilder
            .speak(speakOutput)
            .reprompt(speakOutput)
            .getResponse();
    }
};

const CloseValveIntentHandler = {
    canHandle(handlerInput) {
        return Alexa.getRequestType(handlerInput.requestEnvelope) === 'IntentRequest'
            && Alexa.getIntentName(handlerInput.requestEnvelope) === 'CloseValveIntent';
    },
    handle(handlerInput) {
        var speakOutput = "Debes iniciar sesion con un usuario para continuar";
        try{
            IotData.updateThingShadow(CloseValveParams, function(err, data) {
                if (err) console.log(err);
            });
            speakOutput = 'Cerrando la valvula';
        } catch(error){
            console.error('Error al publicar en shadow:', error);
        }
        return handlerInput.responseBuilder
            .speak(speakOutput)
            .reprompt(speakOutput)
            .getResponse();
    }
};

const QualityAirIntentHandler = {
    canHandle(handlerInput) {
        return Alexa.getRequestType(handlerInput.requestEnvelope) === 'IntentRequest'
            && Alexa.getIntentName(handlerInput.requestEnvelope) === 'QualityAirIntent';
    },
    async handle(handlerInput) {
        let reportedStates;
        var speakOutput = "Debes iniciar sesion con un usuario para continuar";
        try{
            await getShadowPromise(ShadowFireParams).then((result) => reportedStates = result.state.reported);
            speakOutput = `La concentración de '${reportedStates.detected_type}' es de ${reportedStates.ppm_level} ppm`;
        }catch{
            console.error('Error al consultar en shadow:', error)
        }
        return handlerInput.responseBuilder
            .speak(speakOutput)
            .reprompt(speakOutput)
            .getResponse();
    }
};

const HelpIntentHandler = {
    canHandle(handlerInput) {
        return Alexa.getRequestType(handlerInput.requestEnvelope) === 'IntentRequest'
            && Alexa.getIntentName(handlerInput.requestEnvelope) === 'AMAZON.HelpIntent';
    },
    handle(handlerInput) {
        const speakOutput = 'Tienes las opciones de encender, apagar y consultar el estado.';

        return handlerInput.responseBuilder
            .speak(speakOutput)
            .reprompt(speakOutput)
            .getResponse();
    }
};

const CancelAndStopIntentHandler = {
    canHandle(handlerInput) {
        return Alexa.getRequestType(handlerInput.requestEnvelope) === 'IntentRequest'
            && (Alexa.getIntentName(handlerInput.requestEnvelope) === 'AMAZON.CancelIntent'
                || Alexa.getIntentName(handlerInput.requestEnvelope) === 'AMAZON.StopIntent');
    },
    handle(handlerInput) {
        const speakOutput = 'Hasta pronto!';

        return handlerInput.responseBuilder
            .speak(speakOutput)
            .getResponse();
    }
};

const FallbackIntentHandler = {
    canHandle(handlerInput) {
        return Alexa.getRequestType(handlerInput.requestEnvelope) === 'IntentRequest'
            && Alexa.getIntentName(handlerInput.requestEnvelope) === 'AMAZON.FallbackIntent';
    },
    handle(handlerInput) {
        const speakOutput = 'Lo siento, no entendí, intenta de nuevo.';

        return handlerInput.responseBuilder
            .speak(speakOutput)
            .reprompt(speakOutput)
            .getResponse();
    }
};

const SessionEndedRequestHandler = {
    canHandle(handlerInput) {
        return Alexa.getRequestType(handlerInput.requestEnvelope) === 'SessionEndedRequest';
    },
    handle(handlerInput) {
        console.log(`~~~~ Session ended: ${JSON.stringify(handlerInput.requestEnvelope)}`);
        // Any cleanup logic goes here.
        return handlerInput.responseBuilder.getResponse(); // notice we send an empty response
    }
};

const IntentReflectorHandler = {
    canHandle(handlerInput) {
        return Alexa.getRequestType(handlerInput.requestEnvelope) === 'IntentRequest';
    },
    handle(handlerInput) {
        const intentName = Alexa.getIntentName(handlerInput.requestEnvelope);
        const speakOutput = `Intentó ejecutar ${intentName}`;

        return handlerInput.responseBuilder
            .speak(speakOutput)
            //.reprompt('add a reprompt if you want to keep the session open for the user to respond')
            .getResponse();
    }
};

const ErrorHandler = {
    canHandle() {
        return true;
    },
    handle(handlerInput, error) {
        const speakOutput = 'Disculpa, hubo un error. Intenta de nuevo.';
        console.log(`~~~~ Error handled: ${JSON.stringify(error)}`);

        return handlerInput.responseBuilder
            .speak(speakOutput)
            .reprompt(speakOutput)
            .getResponse();
    }
};

export const handler = Alexa.SkillBuilders.custom()
    .addRequestHandlers(
        LaunchRequestHandler,
        UserValidationIntentHandler,
        //Alarm Intruder Intents
        ArmAlarmIntentHandler,
        DisarmAlarmIntentHandler,
        DangerIntentHandler,
        SafeIntentHandler,
        VerifyIntruderIntentHandler,
        //Alarm Fire Intents
        OpenValveIntentHandler,
        CloseValveIntentHandler,
        QualityAirIntentHandler,
        //OTHERS
        HelpIntentHandler,
        CancelAndStopIntentHandler,
        FallbackIntentHandler,
        SessionEndedRequestHandler,
        IntentReflectorHandler)
    .addErrorHandlers(
        ErrorHandler)
    .withCustomUserAgent('sample/hello-world/v1.2')
    .lambda();