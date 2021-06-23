#!/usr/bin/python3

import sys
import pandas as pd
import re
import urlmarker
from nltk.corpus import stopwords

# Define global vars
REPLACE_HASHTAG = re.compile('[#]+[A-Za-z-_]+[A-Za-z0-9-_]')
REPLACE_AT_ENTITIES = re.compile('[@]+[A-Za-z0-9-_]+')
REPLACE_URLS = re.compile(urlmarker.WEB_URL_REGEX)
REPLACE_EMAILS = re.compile('^([a-zA-Z0-9_\-\.]+)@([a-zA-Z0-9_\-\.]+)\.([a-zA-Z]{2,5})$')
REPLACE_BY_SPACE_RE = re.compile('[/(){}\[\]\|@,;]')
REPLACE_DOLLAR_FIGURES = re.compile('[\$]{1}[\d,]+\.?\d{0,2}')
REPLACE_NUMBERS = re.compile('[\d,]+\.?\d{0,2}')
REMOVE_BAD_SYMBOLS = re.compile('[^0-9a-z #+_]')
exclude_list = ['no', 'not']
REMOVE_STOPWORDS = [word for word in set(stopwords.words('english')) if word not in exclude_list]

contractions = {
    "ain't": "is not",
    "aren't": "are not",
    "can't": "cannot",
    "cant": "cannot",
    "can't've": "cannot have",
    "'cause": "because",
    "could've": "could have",
    "couldn't": "could not",
    "couldn't've": "could not have",
    "didn't": "did not",
    "doesn't": "does not",
    "dont":"do not",
    "don't": "do not",
    "hadn't": "had not",
    "hadn't've": "had not have",
    "hasn't": "has not",
    "haven't": "have not",
    "he'd": "he had / he would",
    "he'd've": "he would have",
    "he'll": "he shall / he will",
    "he'll've": "he shall have / he will have",
    "he's": "he has / he is",
    "how'd": "how did",
    "how'd'y": "how do you",
    "how'll": "how will",
    "how's": "how has / how is / how does",
    "I'd": "I had / I would",
    "I'd've": "I would have",
    "I'll": "I shall / I will",
    "I'll've": "I shall have / I will have",
    "I'm": "I am",
    "I've": "I have",
    "isn't": "is not",
    "idk": "i do not know",
    "it'd": "it had / it would",
    "it'd've": "it would have",
    "it'll": "it shall / it will",
    "it'll've": "it shall have / it will have",
    "it's": "it has / it is",
    "let's": "let us",
    "ma'am": "madam",
    "mayn't": "may not",
    "might've": "might have",
    "mightn't": "might not",
    "mightn't've": "might not have",
    "must've": "must have",
    "mustn't": "must not",
    "mustn't've": "must not have",
    "needn't": "need not",
    "needn't've": "need not have",
    "o'clock": "of the clock",
    "oughtn't": "ought not",
    "oughtn't've": "ought not have",
    "shan't": "shall not",
    "sha'n't": "shall not",
    "shan't've": "shall not have",
    "she'd": "she had / she would",
    "she'd've": "she would have",
    "she'll": "she shall / she will",
    "she'll've": "she shall have / she will have",
    "she's": "she has / she is",
    "should've": "should have",
    "shouldn't": "should not",
    "shouldn't've": "should not have",
    "so've": "so have",
    "so's": "so as / so is",
    "that'd": "that would / that had",
    "that'd've": "that would have",
    "that's": "that has / that is",
    "there'd": "there had / there would",
    "there'd've": "there would have",
    "there's": "there has / there is",
    "they'd": "they had / they would",
    "they'd've": "they would have",
    "they'll": "they shall / they will",
    "they'll've": "they shall have / they will have",
    "they're": "they are",
    "they've": "they have",
    "to've": "to have",
    "wasn't": "was not",
    "we'd": "we had / we would",
    "we'd've": "we would have",
    "we'll": "we will",
    "we'll've": "we will have",
    "we're": "we are",
    "we've": "we have",
    "weren't": "were not",
    "what'll": "what shall / what will",
    "what'll've": "what shall have / what will have",
    "what're": "what are",
    "what's": "what has / what is",
    "what've": "what have",
    "when's": "when has / when is",
    "when've": "when have",
    "where'd": "where did",
    "where's": "where has / where is",
    "where've": "where have",
    "who'll": "who shall / who will",
    "who'll've": "who shall have / who will have",
    "who's": "who has / who is",
    "who've": "who have",
    "why's": "why has / why is",
    "why've": "why have",
    "will've": "will have",
    "won't": "will not",
    "won't've": "will not have",
    "would've": "would have",
    "wouldn't": "would not",
    "wouldn't've": "would not have",
    "y'all": "you all",
    "y'all'd": "you all would",
    "y'all'd've": "you all would have",
    "y'all're": "you all are",
    "y'all've": "you all have",
    "yall": "you all",
    "yall'd": "you all would",
    "yall'd've": "you all would have",
    "yall're": "you all are",
    "yall've": "you all have",
    "you'd": "you had / you would",
    "you'd've": "you would have",
    "you'll": "you shall / you will",
    "you'll've": "you shall have / you will have",
    "you're": "you are",
    "you've": "you have"
}

neutral={
    "nan":"0",
    "na":"0",
    "n/a":"0",
    "no comment":"0",
    "no additional comments":"0",
    "none":"0",
    "nothing":"0",
    "not know":"0",
    "n":"0",
    " ":"0"
}

def urlRm(text):
    text = REPLACE_URLS.sub(' ', text)

    return text


def text_prepare(text):
    text = text.lower()  # lowercase text
    if text in contractions.keys(): # replace contractions
        text = contractions.get(text)
    text = REPLACE_URLS.sub(' ', text)  # replace urls with space
    text = REPLACE_HASHTAG.sub(' ', text)  # replace hashtags with space
    text = REPLACE_AT_ENTITIES.sub(' ', text)  # replace @entity with space
    text = REPLACE_EMAILS.sub(' ', text)  # replace emails with space
    text = REPLACE_DOLLAR_FIGURES.sub(' __dollars__ ', text)  # replace dollar amounts with special token
    text = REPLACE_NUMBERS.sub(' __num__ ', text)  # replace dollar amounts with special token
    text = REPLACE_BY_SPACE_RE.sub(' ', text)  # replace REPLACE_BY_SPACE_RE symbols by space in text
    text = REMOVE_BAD_SYMBOLS.sub('', text)  # delete symbols which are in BAD_SYMBOLS_RE from text
    text = text.replace('#', '')  # delete remaining hashtags
    # text = ' '.join([word for word in text.split() if word not in REMOVE_STOPWORDS])  # delete stopwords from text
    text = text.strip()

    if text in neutral.keys(): # replace contractions
        text = neutral.get(text)
        return text

    if len(text.split())<4:
        text=text+" __neutral__"+" __neutral__"+" __neutral__"
        print("sucessfully deployed-------------->"+text)

    return text

def df_prepare(df):
    df['text'] = df.text.apply(lambda x: text_prepare(x.encode('utf-8')))

    return df

def filter_for_modeling(df):
    # Remove blank posts
    is_text_blank = df.text.apply(lambda x: x.isspace() or x == '')
    df = df[~is_text_blank]

    # Remove nan text
    is_nan = df.text.isnull()
    df = df[~is_nan]
    
    # Remove duplicate posts as these lead to bias in model validation:
    is_text_duplicate = df.text.duplicated()
    df = df[~is_text_duplicate]

    return df

# preprocess the text column for nlp analysis
def prep_for_topic_model(df):
    # Remove posts with empty content
    df = df[~df.apply(lambda x: x.text.encode('utf-8').isspace() or x.text == '', axis=1)]

    # apply the nlp tokenization rules
    df = df_prepare(df)

    # filter blank, nan, and duplicate posts (this is a repeat after tokenization rules)
    df = filter_for_modeling(df)

    return (df)

# get label
def get_label(negpost):
    label=str(negpost)
    if negpost == 0:
        label = 'positive'
    elif negpost == 1:
        label = 'negative'

    return label


def add_labels(df):
    df.text = df.apply(lambda x: '__label__' + get_label(x['negpost']) + ' ' + x['text'].encode('utf-8'), axis=1)

    return df


if __name__ == "__main__":
    args = sys.argv
    input = args[1]
    output = args[2]

    # read in the input data
    df_in = pd.read_csv(input, encoding='utf-8', sep=',')

    # modify the text
    df = df_prepare(df_in)

    # filter out useless rows
    df = filter_for_modeling(df)

    # add the attack labels
    df = add_labels(df)

    # save the file
    df.to_csv(output, index=False)