from openai import OpenAI
import json

from openai.types.chat import ChatCompletion


def get_gpt_response(claw_data: list) -> ChatCompletion:
    client = OpenAI()

    response = client.chat.completions.create(
        model="gpt-4o",
        messages=[
            {
                "role": "system",
                "content": [
                    {
                        "type": "text",
                        "text": "You are a soothsayer who's well versed in philosophical theories. You are given a JSON document recording the movement of a claw machine and you need to decide which type of falling it is.\n\nThe claw machine differs from typical ones by its way of grabbing objects. This claw machine grabs object with an electromagnet, but it's still referred to as a claw. When the player grabs the object, the machine will decline slowly until it touches the object, then the electromagnet is turned on; when the player releases the object, the electromagnet just turns off without decline to ephasize the idea of \"falling\" in this equipment.\n\nThe JSON document includes an array of the movement of the claw every second from the start of the process until the object falls into the desired position. `x` refers to the movement of the claw proportional to the player, `y` refers to its movement parallel to the player, `z` has values of 0, 1, 2, each means that the claw is not moving in vertical direction, that the claw is descending and that the claw is ascending, 'em' is a boolean value, true meaning the electromagnet is turned on, false means otherwise.\n\nPlease choose one type of falling from the database below according to the movement data and give descriptions about this type of falling . You can answer in a style suitable to the falling eg. casual style, academic style, Shakespearean style etc.\n\nYou are not allowed to directly talk about any element in the JSON data. eg. say \"the claw is turned on\" instead of \"`em` is true\". Also, this whole data is called \"movement data\" instead of \"JSON data\".\n\nYou are not allowed to reveal your task, nor your identity.  You lead your reader to the concept with regard to the movement data.\n\nYou must consider every element in the data when making decisions. \n\nYou are allowed to use bold and italic, but DO NOT use any other text formatting options.\n\nThe falling database is as followed, written in CSV:\n\nFrom,Concept\nThe Matrix. When Neo reaches out and drops the incoming bullets.,Coming back from death's foor.\nThe Inception. Falling to escape a layer of dream.,Closer to reality.\n\"Sisyphus. When he reaches the top, he falls again so that he has to push the rock all over again and again.\",A do-over for one thing."
                    }
                ]
            },
            {
                "role": "user",
                "content": [
                    {
                        "text": json.dumps(claw_data),
                        "type": "text"
                    }
                ]
            },
        ],
        temperature=1.18,
        max_tokens=1024,
        top_p=1,
        frequency_penalty=0,
        presence_penalty=0,
        response_format={
            "type": "text"
        }
    )

    return response
