from src.include.PyCLink import pyclink
import sys

def chatgpt():
    pass

#uzklausa = pyclink.receiveInput()
uzklausa = "labas chatgpt"
atsakymas = chatgpt() 

pyclink.send(pyclink.TYPES.CHAR_P, atsakymas)