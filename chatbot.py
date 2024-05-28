from src.include.PyCLink import pyclink
import sys
#pries tai reikia 'pip install requests', kas tas pip
import requests #ar openai
import os

openai_api_key = 'mano api key'

# Define the headers and the payload
headers = {
    'Authorization': f'Bearer {api_key}',
    'Content-Type': 'application/json'
}
data = {
    "model": "gpt-3.5-turbo",
    "messages": [
        {"role": "system", "content": "You are a helpful assistant."},
        {"role": "user", "content": "How do I integrate GPT-3.5 Turbo into my code?"}
    ]
}
response = requests.post('https://api.openai.com/v1/chat/completions', headers=headers, json=data)

if response.status_code == 200:
    reply = response.json()
    print(reply['choices'][0]['message']['content'])
else:
    print(f"Error: {response.status_code}")
    print(response.json())
 



 
def chatgpt():
    pass

#uzklausa = pyclink.receiveInput()
uzklausa = "labas chatgpt"
atsakymas = chatgpt() 




def get_openai_response (prompt, model="text-davinci-003", max_tokens=150); #gali skirtis juk model ir token sk
response = openai.Completion.create(model=model, prompt=prompt, max_tokens=max_tokens);
return
response.choices[0].text.strip()

#prompt= blabla babla
#response= get_openai_response (prompt)
#print(response)
 


#import def return 
pyclink.send(pyclink.TYPES.CHAR_P, atsakymas)