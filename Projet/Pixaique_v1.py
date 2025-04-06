import tkinter as tk
from tkinter import ttk, filedialog
import subprocess
from PIL import Image, ImageTk
import os
import platform

selected_image = None
image_label = None
output_image_path = None

# Couleurs et styles
theme_bg = "#6a89cc" #Fond
theme_fg = "#0a3d62" #Police
# theme_accent = "#e55039"
theme_button = "#82ccdd"
# theme_border = "#eb2f06"

# ================================= FONCTIONS =================================

# Lancer le programme
def on_launch():
    global selected_image
    if selected_image:
        method = method_combo.get()
        block_size = taille_combo.get()
        num_images = bb_combo.get()
        mode = mode_combo.get()

        if mode == "Gris":
            input_pnm = "output.pgm"
            output_pnm = "output.pgm"
            output_png = "output.png"
            convert_cmd = ["convert",selected_image,f"pgm:{input_pnm}"]
            executable = "./exe/" + methode(method)
        elif mode == "Couleur":
            if num_images == "10000":
                num_images = "9765"
            input_pnm = "output.ppm"
            output_pnm = "output.ppm"
            output_png = "output.png"
            convert_cmd = ["convert",selected_image,f"ppm:{input_pnm}"]
            executable = "./exe/" + methode(method) + "_RGB"

        try:
            print(f"Conversion de {selected_image} en {'PGM' if mode == 'Gris' else 'PPM'}...")
            subprocess.run(convert_cmd,check=True)

            print(f"Exécution de : {executable}")
            command = [executable,input_pnm,output_pnm,block_size,num_images]
            subprocess.run(command,check=True)

            print(f"Conversion de {output_pnm} en PNG...")
            subprocess.run(["convert",output_pnm,output_png],check=True)

            path(output_png)
            show_image(output_png)

        except subprocess.CalledProcessError as e:
            print("Erreur subprocess :", e)

def methode(method):
    if method == "Moyenne":
        return "mosaique_moy"
    elif method == "Histogramme":
        return "mosaique_histo"
    elif method == "Spécification":
        return "mosaique_specifier"

# Ajouter une image
def on_add_image():
    global selected_image, image_label
    file_path = filedialog.askopenfilename(filetypes=[("PNG files","*.png")])
    if file_path:
        selected_image = file_path
        print(f"Image sélectionnée : {file_path}")
        img = Image.open(selected_image)
        img.thumbnail((500,500))
        img_tk = ImageTk.PhotoImage(img)
        
        if image_label:
            image_label.config(image=img_tk)
            image_label.image = img_tk
        else:
            image_label = tk.Label(image_frame,image=img_tk,relief="solid",bg=theme_bg)
            image_label.image = img_tk
            image_label.pack(padx=10,pady=5)

def path(output_path):
    global output_image_path
    output_image_path = output_path
    print(f"Chemin de l'image de sortie : {output_path}")

def show_image(path):
    res = tk.Toplevel(root)
    res.title("Résultat")
    res.configure(bg=theme_bg)

    try:
        result_img = Image.open(path)
        result_img.thumbnail((1000,1000))
        img_tk = ImageTk.PhotoImage(result_img)

        label = tk.Label(res,image=img_tk,bg=theme_bg)
        label.image = img_tk
        label.pack(padx=10,pady=10)

        close_button = tk.Button(res,text="Fermer",command=res.destroy,bg=theme_button,fg=theme_fg,font=("Arial",16,"bold"),relief="ridge")
        close_button.pack(pady=10)
    except Exception as e:
        print(f"Erreur affichage popup : {e}")

# Ouvrir l'image avec la visionneuse du système
def open_image():
    global output_image_path
    if output_image_path and os.path.exists(output_image_path):
        system = platform.system()
        try:
            if system == "Windows":
                os.startfile(output_image_path)
            elif system == "Darwin":  # macOS
                subprocess.run(["open",output_image_path])
            else:  # Linux
                subprocess.run(["xdg-open",output_image_path])
            print(f"Image ouverte : {output_image_path}")
        except Exception as e:
            print(f"Impossible d'ouvrir l'image : {e}")
    else:
        print("Aucune image à ouvrir.")

def on_close():
    root.destroy()

# ================================= INTERFACE =================================

root = tk.Tk()
root.title("Pixaïque")
root.geometry("1100x675")
root.resizable(False,False)
root.configure(bg=theme_bg)

# Cadre principal
main_frame = tk.Frame(root,padx=10,pady=10,bg=theme_bg)
main_frame.pack(expand=True,fill="both")

# Cadre gauche
left_frame = tk.Frame(main_frame,width=300,padx=10,pady=10,bg=theme_bg)
left_frame.pack(side=tk.LEFT,fill=tk.Y)

# Cadre droit
right_frame = tk.Frame(main_frame,padx=10,pady=10,bg=theme_bg)
right_frame.pack(side=tk.LEFT,expand=True,fill='both')

tk.Label(left_frame,text="Pixaïque",font=("Arial",16,"bold"),bg=theme_bg,fg=theme_fg).pack(pady=(0,20))

# Méthode
tk.Label(left_frame,text="Méthode",font=("Arial",14),bg=theme_bg,fg=theme_fg).pack(anchor='w')
method_combo = ttk.Combobox(left_frame,values=["Moyenne","Histogramme","Spécification"],state="readonly",font=("Arial",16))
method_combo.current(0)
method_combo.pack(fill="x", pady=5)

# Mode
tk.Label(left_frame,text="Mode",font=("Arial",14),bg=theme_bg,fg=theme_fg).pack(anchor='w')
mode_combo = ttk.Combobox(left_frame,values=["Gris","Couleur"],state="readonly",font=("Arial",16))
mode_combo.set("Gris")
mode_combo.pack(fill="x",pady=5)

# Taille Bloc
tk.Label(left_frame, text="Taille bloc",font=("Arial",14),bg=theme_bg,fg=theme_fg).pack(anchor='w')
taille_combo = ttk.Combobox(left_frame,values=["32","16","8"],state="readonly",font=("Arial",16))
taille_combo.current(0)
taille_combo.pack(fill="x", pady=5)

# Nombre d'imagettes
tk.Label(left_frame,text="Nombre imagettes",font=("Arial",14),bg=theme_bg,fg=theme_fg).pack(anchor='w')
bb_combo = ttk.Combobox(left_frame,values=["100","1000","10000"],state="readonly",font=("Arial",16))
bb_combo.current(0)
bb_combo.pack(fill="x", pady=5)

launch_button = tk.Button(left_frame,text="Lancer !",command=on_launch,bg=theme_button,fg=theme_fg,font=("Arial",16,"bold"), relief="ridge")
launch_button.pack(pady=15)

image_frame = tk.Frame(right_frame,relief="ridge",borderwidth=2,bg=theme_bg)
image_frame.pack(pady=10,padx=10,fill="both",expand=True)

add_button = tk.Button(right_frame,text="Ajouter une image",command=on_add_image,bg=theme_button,fg=theme_fg,font=("Arial",16,"bold"),relief="ridge")
add_button.pack(pady=15)

root.mainloop()