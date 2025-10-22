# Usage
1. Download latest unicode data:
    - shared
        - https://www.unicode.org/Public/UCD/latest/ucd/UnicodeData.txt
    - composition
        - https://www.unicode.org/Public/UCD/latest/ucd/CompositionExclusions.txt
        - https://www.unicode.org/Public/UCD/latest/ucd/NormalizationTest.txt
    - case
        - https://www.unicode.org/Public/UCD/latest/ucd/SpecialCasing.txt
        - https://www.unicode.org/Public/UCD/latest/ucd/CaseFolding.txt
2. Put it in this folder.
3. `odin run unicode/gen`
4. `odin test unicode` TODO: write some tests
