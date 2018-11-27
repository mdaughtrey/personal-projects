typeof:{[x]
    `0: $(`"vector symbol"
        `"vector character"
        `"vector float"
        `"vector integer"
        `"list general"
        `"scalar integer"
        `"scalar float"
        `"scalar character"
        `"scalar symbol"
        `"dictionary"
        `"nil"
        `"verb/function"
        )[4+4:x]
}



cards: ((`Ace;11);(`Two;2);(`Three;3);(`Four;4);(`Five;5);(`Six;6);(`Seven;7);(`Eight;8);(`Nine;9);(`Jack;10);(`Queen;10);(`King;10))
suits: `Hearts `Clubs `Spades `Diamonds

debug: {`0: " "/($:/x)}
deal: {
    res[`suit]: suits@1?#suits / draw random suit
    res[`card]: cards[1?#cards] }

    

