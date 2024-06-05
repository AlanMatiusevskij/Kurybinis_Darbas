from openai import OpenAI
from src.include.PyCLink import pyclink

api_key = ""
client = OpenAI(api_key=api_key)

def CHATGPT_UZKLAUSA(uzklausa):
  inp = "If possible, keep the answer on the short side: " + uzklausa
  completion = client.chat.completions.create(
    model="gpt-3.5-turbo-0125",
    messages=[
      {"role": "user", "content": inp}
    ]
  )
  return completion.choices[0].message.content

atsakymas = None

if pyclink.checkForArgs() == False:
  print("Launched without arguments!\nQuit or type your question.")
  inputas = input()
  atsakymas = CHATGPT_UZKLAUSA(inputas)
  print(atsakymas)
  input()
else:
  atsakymas = CHATGPT_UZKLAUSA(pyclink.receiveInput()) 
  pyclink.send(pyclink.TYPES.CHAR_P, atsakymas)