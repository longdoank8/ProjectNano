import pandas as pd
import numpy as np
import matplotlib
import matplotlib.pyplot as plt
import common
labels = ['sent at 1 min', 'sent at 4 min', 'sent at 7 min']
#amount of vessel -gateway heart
# bot_1000 = [19/94, 31/94, 24/94]
# bot_6500 = [30/94, 64/94, 65/94]

#amount of vessels- gateway leftforearm
# bot_1000 = [39/94, 31/94, 33/94]
# bot_6500 = [59/94, 53/94, 62/94]

#hop counts -gateway heart
# bot_1000 = [125.73573573573573, 39.07, 51.701]
# bot_1000_error = [66.99023700484775, 29.419017294576285, 39.292945098764676]
# bot_6500 = [395.56876923076925, 154.90646153846154, 152.894]
# bot_6500_error = [268.8807591373453, 145.743833432604, 133.68857991754302]

#hop count - gateway left forearm
bot_1000 = [47.16, 59.23, 64.386]
bot_1000_error = [18.0976773627426, 30.36641597586418, 30.505238461181932]
bot_6500 = [466.7943076923077, 384.17830769230767, 356.3715384615385]
bot_6500_error = [248.3513449127774, 191.96126831690188, 152.6666357365629]


x = np.arange(len(labels))
width = 0.35  # the width of the bars

# common.set_plotting_style(
#     style='paper-color',
#     enable_pgf=True,
# )
# figsize = common.get_latex_fig_size(
#     scale=1,
#     ratio=9 / 18,
#     text_width_pt=common.style_default_width_pt,
#     dpi=common.style_dpi,
# )
fig, ax = plt.subplots()#figsize=figsize)

fig, ax = plt.subplots()
rects1 = ax.bar(x - width/2, bot_1000, width, yerr=bot_1000_error, label='1000 Bots', color='blue', ecolor='black', capsize=10)
rects2 = ax.bar(x + width/2, bot_6500, width, yerr=bot_6500_error, label='6500 Bots', color='cornflowerblue', ecolor='black', capsize=10)
# rects1 = ax.bar(x - width/2, bot_1000, width, label='1000 Bots', color='blue')
# rects2 = ax.bar(x + width/2, bot_6500, width, label='6500 Bots', color='cornflowerblue')

# Add some text for labels, title and custom x-axis tick labels, etc.
ax.set_ylabel('Hop Count', fontsize=9)
# ax.set_ylabel('Fraction of Vessels', fontsize=9)
# ax.set_xlabel(fontsize=15)
# plt.xticks(fontsize=10, rotation=0)
plt.yticks(fontsize=9, rotation=0)
ax.set_title('average amount of hops needed to reach nanobots ')
ax.set_xticks(x)
ax.set_xticklabels(labels, fontsize=9)
# ax.legend()
# ax.set_ylim(top=500)
ax.set_ylim(top=750)
# ax.axhline(
#             y=94,
#             color='k',
#             linewidth=.5,
#             linestyle='--',
#         )
plt.legend(loc=(0.79,0.88),fontsize=9)

def autolabel(rects):
    """Attach a text label above each bar in *rects*, displaying its height."""
    for rect in rects:
        height = rect.get_height()
        ax.annotate('{}'.format(height),
                    xy=(rect.get_x() + rect.get_width() / 2, height),
                    xytext=(0, 3),  # 3 points vertical offset
                    textcoords="offset points",
                    ha='center', va='bottom')


# autolabel(rects1)
# autolabel(rects2)

fig.tight_layout(pad=0.3)

plt.show()
fig.savefig("hop_count_leftforearm.png")

