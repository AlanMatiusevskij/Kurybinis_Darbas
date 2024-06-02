from openai import OpenAI

api_key = ""
client = OpenAI(api_key=api_key)

completion = client.chat.completions.create(
  model="gpt-3.5-turbo-0125",
  messages=[
    {"role": "system", "content": "Say 'egg'"}
  ]
)

print(completion.choices[0].message)
