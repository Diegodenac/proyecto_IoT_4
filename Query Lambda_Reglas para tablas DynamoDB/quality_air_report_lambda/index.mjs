import { DynamoDBClient } from "@aws-sdk/client-dynamodb";
import { PutCommand, DynamoDBDocumentClient } from "@aws-sdk/lib-dynamodb";

const client = new DynamoDBClient({});
const docClient = DynamoDBDocumentClient.from(client);

export const handler = async (event, context) => {
    const command = new PutCommand({
        TableName: "quality_air_data",
        Item: {
            serial_number: event.serial,
            timestamp: event.timestamp,
            thing_name: event.thing_name,
            ppm_concetration: event.ppm_level,
            gas_type: event.detected_type,
        },
    });

    const response = await docClient.send(command);
    console.log(response);
};
