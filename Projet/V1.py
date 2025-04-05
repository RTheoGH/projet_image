import tkinter as tk
from tkinter import ttk, filedialog
import subprocess
from PIL import Image, ImageTk

selected_image = None
image_label = None

# Couleurs et styles
theme_bg = "#2E3440"
theme_fg = "#D8DEE9"
theme_accent = "#88C0D0"
theme_button = "#5E81AC"
theme_border = "#4C566A"

# Lancer le programme
def on_launch():
    global selected_image
    if selected_image:
        method = method_combo.get()
        block_size = taille_combo.get()
        num_images = bb_combo.get()
        output_image_pgm = "output.pgm"
        output_image_png = "output.png"

        png_to_pgm_command = ["convert", selected_image, "pgm:output.pgm"]
        try:
            print(f"Conversion de {selected_image} en PGM...")
            subprocess.run(png_to_pgm_command, check=True)

            command = ["./exe/mosaique_moy", "output.pgm", output_image_pgm, block_size, num_images]
            print(f"Exécution de la commande : {command}")
            subprocess.run(command, check=True)

            pgm_to_png_command = ["convert", output_image_pgm, output_image_png]
            print(f"Conversion de {output_image_pgm} en PNG...")
            subprocess.run(pgm_to_png_command, check=True)

            print("Subprocess terminé")
            display_result(output_image_png)
        except subprocess.CalledProcessError as e:
            print("Erreur lors de l'exécution :", e)

# Ajouter une image
def on_add_image():
    global selected_image, image_label
    file_path = filedialog.askopenfilename(filetypes=[("PNG files", "*.png")])
    if file_path:
        selected_image = file_path
        print(f"Image sélectionnée : {file_path}")
        img = Image.open(selected_image)
        img.thumbnail((300, 300))
        img_tk = ImageTk.PhotoImage(img)
        
        if image_label:
            image_label.config(image=img_tk)
            image_label.image = img_tk
        else:
            image_label = tk.Label(image_frame, image=img_tk, relief="solid", bg=theme_bg)
            image_label.image = img_tk
            image_label.pack(padx=10, pady=5)

def display_result(output_path):
    print(f"Chemin de l'image de sortie : {output_path}")
    try:
        result_img = Image.open(output_path)
        result_img.thumbnail((200, 200))
        result_img_tk = ImageTk.PhotoImage(result_img)
        result_label.config(image=result_img_tk)
        result_label.image = result_img_tk
    except Exception as e:
        print(f"Erreur lors de l'affichage du résultat : {e}")

def on_close():
    root.destroy()

root = tk.Tk()
root.title("Pixaïque")
root.geometry("1600x1000")
root.minsize(700, 550)
root.configure(bg=theme_bg)

# Cadre principal
main_frame = tk.Frame(root, padx=10, pady=10, bg=theme_bg)
main_frame.pack(expand=True, fill="both")

# Cadre supérieur (paramètres)
top_frame = tk.Frame(main_frame, bg=theme_bg)
top_frame.pack(fill="x")

def create_label(parent, text):
    return tk.Label(parent, text=text, fg=theme_fg, bg=theme_bg, font=("Arial", 12, "bold"))

def create_button(parent, text, command):
    return tk.Button(parent, text=text, command=command, bg=theme_button, fg=theme_fg, font=("Arial", 12, "bold"), relief="ridge")

method_label = create_label(top_frame, "Méthode:")
method_label.pack(side="left", padx=5)
method_combo = ttk.Combobox(top_frame, values=["Moyenne", "Histogramme", "Spécification"])
method_combo.pack(side="left", padx=5)
method_combo.current(0)

taille_label = create_label(top_frame, "Taille Bloc:")
taille_label.pack(side="left", padx=5)
taille_combo = ttk.Combobox(top_frame, values=["32", "16", "8"])
taille_combo.pack(side="left", padx=5)
taille_combo.current(0)

nb_label = create_label(top_frame, "Nb Imagettes:")
nb_label.pack(side="left", padx=5)
bb_combo = ttk.Combobox(top_frame, values=["100", "1000", "10000"])
bb_combo.pack(side="left", padx=5)
bb_combo.current(0)

# Cadre image
image_frame = tk.Frame(main_frame, relief="ridge", borderwidth=2, bg=theme_bg)
image_frame.pack(pady=10)

add_button = create_button(image_frame, "Ajouter une image", on_add_image)
add_button.pack(pady=10)

# Cadre résultat
result_frame = tk.Frame(main_frame, relief="ridge", borderwidth=2, bg=theme_bg)
result_frame.pack(pady=10)

result_label = tk.Label(result_frame, text="Résultat", width=20, height=10, relief="solid", bg=theme_bg, fg=theme_fg)
result_label.pack(padx=10, pady=10)

# Boutons d'action
button_frame = tk.Frame(main_frame, bg=theme_bg)
button_frame.pack(fill="x", pady=10)

launch_button = create_button(button_frame, "Lancer !", on_launch)
launch_button.pack(side="right", expand=True, padx=10)

close_button = create_button(button_frame, "Fermer", on_close)
close_button.pack(side="left", expand=True, padx=10)

root.mainloop()
