import sys
from ultralytics import YOLO

if len(sys.argv) != 2:
    print("Uso: python classificar.py imagem.jpg")
    exit(1)

imagem = sys.argv[1]
model = YOLO("best01.pt")  # Seu modelo treinado para classificação

# Faz a predição
results = model(imagem)[0]

if results.probs is None:
    saida = "Nenhuma classe foi prevista."
else:
    nomes = model.names  # dicionário {índice: nome}
    probabilidades = results.probs.data.tolist()

    # Pega os top-5 índices com maiores probabilidades
    top5_indices = sorted(range(len(probabilidades)), key=lambda i: probabilidades[i], reverse=True)[:5]

    saida = "Top 5 classificações:\n"
    for i in top5_indices:
        saida += f"- {nomes[i]}: {probabilidades[i] * 100:.2f}%\n"

# Escreve no arquivo resultado.txt
with open("resultado.txt", "w", encoding="utf-8") as f:
    f.write(saida)
