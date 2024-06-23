const int k_velveteenLength1 = 7;
const char* txt_velveteen1[k_velveteenLength1] =
  {"HERE was once a velveteen rabbit, and in the beginning he was really splendid. He was fat and bunchy, as a rabbit should be;",
  "his coat was spotted brown and white, he had real thread whiskers, and his ears were lined with pink sateen.",
  "On Christmas morning, when he sat wedged in the top of the Boy's stocking, with a sprig of holly between his paws, the effect was charming.",  
  "There were other things in the stocking, nuts and oranges and a toy engine, and chocolate almonds and a clockwork mouse,",
  "but the Rabbit was quite the best of all. For at least two hours the Boy loved him, and then Aunts and Uncles came to dinner,",
  "and there was a great rustling of tissue paper and unwrapping of parcels, and in the excitement of looking at all the new presents", 
  "the Velveteen Rabbit was forgotten."
};

const int k_velveteenLength2 = 12;
const char* txt_velveteen2[k_velveteenLength2] = 
{
  "Christmas Morning",  
  "For a long time he lived in the toy cupboard or on the nursery floor, and no one thought very much about him.",
  "He was naturally shy, and being only made of velveteen, some of the more expensive toys quite snubbed him.",
  "The mechanical toys were very superior, and looked down upon every one else; they were full of modern ideas, and pretended they were real.",
  "The model boat, who had lived through two seasons and lost most of his paint, caught the tone from them",
  "and never missed an opportunity of referring to his rigging in technical terms.",
  "The Rabbit could not claim to be a model of anything, for he didn't know that real rabbits existed;",
  "he thought they were all stuffed with sawdust like himself, and he understood that sawdust was quite out-of-date",
  "and should never be mentioned in modern circles. Even Timothy, the jointed wooden lion, who was made by the disabled soldiers,",
  "and should have had broader views, put on airs and pretended he was connected with Government.",
  "Between them all the poor little Rabbit was made to feel himself very insignificant and commonplace,",
  "and the only person who was kind to him at all was the Skin Horse."
};



// TODO: I removed the accents and enes because the font doesn't support them. Add them back in once we make a new font
const int k_versosSencillosLength = 3;
const char* txt_VersosSencillos[k_versosSencillosLength] = 
{
  "Yo soy un hombre sincero / De donde crece la palma, / Y antes de morirme quiero / Echar mis versos del alma.",
  "Yo vengo de todas partes, / Y hacia todas partes voy: / Arte soy entre las artes, / En los montes, monte soy.",
  "Yo se los nombres extraños / De las yerbas y las flores, / Y de mortales engaños, / Y de sublimes dolores."
};

const int k_numReadingAssets = 3;
const char** allReadingAssets[k_numReadingAssets] = {txt_velveteen1, txt_velveteen2, txt_VersosSencillos};
const int allReadingAssetLengths[k_numReadingAssets] = {k_velveteenLength1, k_velveteenLength2, k_versosSencillosLength};