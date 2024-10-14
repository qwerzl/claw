import markdown
from PIL import Image
from escpos.printer import Network
from playwright.sync_api import sync_playwright

def print_img(image: str, crop: bool = True):
    image_toprint = image

    img = Image.open(open(image_toprint, 'rb'))
    # summarize some details about the image
    print("Image format: \'{}\'".format(img.format))
    print("Image mode: \'{}\'".format(img.mode))
    print("Image size: \'{}\'".format(img.size))

    prn_dpi = 203 # check out printer's specifications
    inches_width = 2.83465 # width of the adhesive paper roll, in inches (72mm for 80mm roll)

    # convert to grayscale
    img = img.convert('1')

    # resize to fit the paper width:
    maxwidth = int(prn_dpi * inches_width)
    currwidth = img.size[0]
    currheight = img.size[1]
    print("Max allowed width is: {}px, actual width is {}px".format(maxwidth, currwidth))
    scaling_ratio = maxwidth / currwidth
    print("Scaling factor needs to be {}%".format(int(scaling_ratio*100)))
    if scaling_ratio < 1:
        img = img.resize((maxwidth,int(currheight * scaling_ratio)), Image.BILINEAR)
        print("Resized to: {}px × {}px".format(maxwidth,int(currheight * scaling_ratio)))
    else:
        print("No downscaling was required, will leave image as-is.")

    # fixes issue with poorly printed top margin (adds spare 5px on top)
    nwidth, nheight = img.size
    margin = 5
    new_height = nheight + margin
    print("Size with margin: {}px × {}px".format(nwidth,new_height))
    fix = Image.new("L", (nwidth, new_height), (255))
    fix.paste(img, (0, margin))
    img = fix

    # converts canvas to BW (better we do it here than rely on printer's firmware)
    img = img.convert('1')
    img.save(image_toprint, dpi=(prn_dpi, prn_dpi)  )

    p = Network("10.249.19.159")
    p.set(align=u'center')
    print("Printing image..")
    p.image(image_toprint)
    if crop:
        print("Cropping paper")
        p.cut(mode='FULL')
    else:
        print("Cropping paper is disabled.")
    print("Finished.")

def print_markdown(text: str):
    output = markdown.markdown(text)

    output += '''
<style>
body {
    font-family: "Courier Prime", serif;
}
* {
    font-size: 30px;
}
</style>
    '''

    print(output)

    # imgkit.from_string(output, 'out.png', options={
    #     'format': 'png',
    #     'encoding': "UTF-8",
    #     'width': '575',
    # })

    with sync_playwright() as p:
        browser = p.chromium.launch()
        page = browser.new_page()
        page.set_viewport_size({"width": 575, "height": 800})
        page.set_content(output)

        page.screenshot(path='./output.png', full_page=True)
        # locator = page.locator("body")
        # height = locator.evaluate("el => el.scrollHeight")  # Get the element's scroll height
        # viewport_size = page.viewport_size  # Get the current viewport size
        #
        # # Check if resizing is needed
        # needs_resize = height > viewport_size['height']
        # if needs_resize:
        #     page.set_viewport_size({"width": viewport_size['width'], "height": height})
        #
        # # Take the screenshot
        # locator.screenshot(path='./output.png')
        #
        # # Restore the original viewport size if it was resized
        # if needs_resize:
        #     page.set_viewport_size(viewport_size)

    print_img("output.png", crop=True)

# print_markdown('''
# ## Overview
#
# ### Philosophy
#
# Markdown is intended to be as easy-to-read and easy-to-write as is feasible.
#
# Readability, however, is emphasized above all else. A Markdown-formatted
# document should be publishable as-is, as plain text, without looking
# like it's been marked up with tags or formatting instructions. While
# Markdown's syntax has been influenced by several existing text-to-HTML
# filters -- including [Setext](http://docutils.sourceforge.net/mirror/setext.html), [atx](http://www.aaronsw.com/2002/atx/), [Textile](http://textism.com/tools/textile/), [reStructuredText](http://docutils.sourceforge.net/rst.html),
# [Grutatext](http://www.triptico.com/software/grutatxt.html), and [EtText](http://ettext.taint.org/doc/) -- the single biggest source of
# inspiration for Markdown's syntax is the format of plain text email.
#
# ## Block Elements
#
# ### Paragraphs and Line Breaks
#
# A paragraph is simply one or more consecutive lines of text, separated
# by one or more blank lines. (A blank line is any line that looks like a
# blank line -- a line containing nothing but spaces or tabs is considered
# blank.) Normal paragraphs should not be indented with spaces or tabs.
#
# The implication of the "one or more consecutive lines of text" rule is
# that Markdown supports "hard-wrapped" text paragraphs. This differs
# significantly from most other text-to-HTML formatters (including Movable
# Type's "Convert Line Breaks" option) which translate every line break
# character in a paragraph into a `<br />` tag.
#
# When you *do* want to insert a `<br />` break tag using Markdown, you
# end a line with two or more spaces, then type return.
#
# ### Headers
#
# Markdown supports two styles of headers, [Setext] [1] and [atx] [2].
#
# Optionally, you may "close" atx-style headers. This is purely
# cosmetic -- you can use this if you think it looks better. The
# closing hashes don't even need to match the number of hashes
# used to open the header. (The number of opening hashes
# determines the header level.)
#
#
# ### Blockquotes
#
# Markdown uses email-style `>` characters for blockquoting. If you're
# familiar with quoting passages of text in an email message, then you
# know how to create a blockquote in Markdown. It looks best if you hard
# wrap the text and put a `>` before every line:
#
# > This is a blockquote with two paragraphs. Lorem ipsum dolor sit amet,
# > consectetuer adipiscing elit. Aliquam hendrerit mi posuere lectus.
# > Vestibulum enim wisi, viverra nec, fringilla in, laoreet vitae, risus.
# >
# > Donec sit amet nisl. Aliquam semper ipsum sit amet velit. Suspendisse
# > id sem consectetuer libero luctus adipiscing.
#
# Markdown allows you to be lazy and only put the `>` before the first
# line of a hard-wrapped paragraph:
#
# > This is a blockquote with two paragraphs. Lorem ipsum dolor sit amet,
# consectetuer adipiscing elit. Aliquam hendrerit mi posuere lectus.
# Vestibulum enim wisi, viverra nec, fringilla in, laoreet vitae, risus.
#
# > Donec sit amet nisl. Aliquam semper ipsum sit amet velit. Suspendisse
# id sem consectetuer libero luctus adipiscing.
#
# Blockquotes can be nested (i.e. a blockquote-in-a-blockquote) by
# adding additional levels of `>`:
#
# > This is the first level of quoting.
# >
# > > This is nested blockquote.
# >
# > Back to the first level.
#
# Blockquotes can contain other Markdown elements, including headers, lists,
# and code blocks:
#
# > ## This is a header.
# >
# > 1.   This is the first list item.
# > 2.   This is the second list item.
# >
# > Here's some example code:
# >
# >     return
#
# Any decent text editor should make email-style quoting easy. For
# example, with BBEdit, you can make a selection and choose Increase
# Quote Level from the Text menu.
# ''')
