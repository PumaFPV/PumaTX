from tkinter import *

root = Tk()
root.title('PumaTX Configurator')
root.iconbitmap('C:/Users/Nico/Pictures/Puma/puma_black.ico')
img = PhotoImage(file="C:/Users/Nico/Desktop/pumatx/Software/PumaTX_Configurator_py/img/display_full.png")
root.geometry("720x640")


display = Canvas(root, width=650+10, height=340+10, bg='#282c34')
display.pack(pady=20)

my_image = display.create_image(833, 442, anchor=NW, image=img)

display.create_line(5, 5, 655, 5, fill="white")
display.create_line(655, 5, 655, 205, fill="white")
display.create_line(655, 205, 490, 345, fill="white")
display.create_line(490, 345, 165, 345, fill="white")
display.create_line(165, 345, 5, 205, fill="white")
display.create_line(5, 205, 5, 5, fill="white")
display.create_polygon()

root['background']='#3c3f41'
root.mainloop()