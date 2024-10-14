from openai import OpenAI
import json

from openai.types.chat import ChatCompletion


def get_gpt_response(claw_data: list) -> ChatCompletion:
    with open("server/contents/prompt.md", "r") as file:
        prompt = file.read()

    client = OpenAI()

    response = client.chat.completions.create(
        model="gpt-4o",
        messages=[
            {
                "role": "system",
                "content": [
                    {
                        "type": "text",
                        "text": prompt
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
