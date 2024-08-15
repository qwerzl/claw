import pandas as pd
from openai import OpenAI

client = OpenAI()


def get_embedding(text, model="text-embedding-3-small"):
    text = text.replace("\n", " ")
    return client.embeddings.create(input=[text], model=model).data[0].embedding


# load & inspect dataset
input_datapath = "./data/concepts.csv"  # to save space, we provide a pre-filtered dataset
df = pd.read_csv(input_datapath)
print(df)
df["combined"] = (
        "Concept: " + df.Concept.str.strip() + "; From: " + df.From.str.strip()
)
df['ada_embedding'] = df.combined.apply(lambda x: get_embedding(x, model='text-embedding-3-small'))
df.to_csv('./data/output/embedded_1k_reviews.csv', index=False)
