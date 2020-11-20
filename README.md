# Ace3x

![](/screenshots/image-viewer.png)

See 'screenshots/' folder for more.

# Progress

## Reading of archives

VPP and PEG mostly read just fine, but some entries are skipped for various reasons:

- They wrongly report their file size
- Their file size would mean that it exceeds the length of the archive
- Their filename is corrupted or missing

This is possibly due to the way I read the archives, and not them being wrongly encoded.

## PEG

There is one image format that I am unable to decode, with format code 0x2.

## ARR, TBL

These are plaintext files and used for various purposes, such as file lists and scripts.

## VAP, RFX, VSE, VMU

These formats I can't decode.

- VAP appears to be related to animation.
- RFX is for visual effects.
- VSE and VMU are sound formats.
- V3D is mentioned a lot but no files have this extension. Instead they
	are named "maia_v3d.peg" and the like. It was likely an intermediate format
	that was processed into the output files.

## P3D

This is a compressed version of S3D used in alternate versions of Summoner, specifically for the PS2.
S3D is a map/level format.

This format is partially decoded, but I can't figure out how to get geometry data from it.

## VIM

Although there are files relating to this format in Ace3x, I can hardly even consider it partially decoded.
