import pandas as pd
import matplotlib.pyplot as plt
import matplotlib as mpl

import common #local script

def main():
    common.set_plotting_style(
        style='paper-color',
        enable_pgf=True,
    )
    figsize = common.get_latex_fig_size(
        scale=1,
        ratio=9/18,
        text_width_pt=common.style_default_width_pt,
        dpi=common.style_dpi,
    )
    fig, ax = plt.subplots(figsize=figsize)

    nums_bots = 1000
    nums_message_at = [60, 240, 420]

    stats = dict()
    legend_lines = []
    legend_labels = []

    for i, num_message_at in enumerate(nums_message_at):
        df = process_ecdf(data_filename=f"csvNano1000_messageat{num_message_at}sec.csv", num_message=num_message_at)
        linestyle = '-'  # no dashes in this case
        color = 'midnightblue'
        if i == 1:
            color = 'mediumblue'
        elif i ==2 :
            color = 'darkturquoise'
        # color = common.style_color_cat_palette[i]
        df.plot(
            x='time_received',
            y='fraction_received',
            ax=ax,
            color=color,
            linestyle=linestyle,
            legend=False,
        )
        stats[nums_bots] = dict(
            time_first_received=df.time_received.min(),
            time_fifty_percent_received=(
                df.query('fraction_received > .5').time_received.min()
            ),
            time_last_received=df.time_received.max()
        )
        legend_lines.append(
            mpl.lines.Line2D([0], [0], color=color, linestyle=linestyle)
        )
        legend_labels.append(f"message sent at {num_message_at/60} min")

        ax.legend(
            legend_lines,
            legend_labels,
            loc='lower right',
        )
        ax.set_xlabel("Latency in s")
        ax.set_ylabel("Nanobots reached, ECDF")
        ax.set_ylim(bottom=0)
        ax.set_xlim(left=0, right=60)
        ax.axhline(
            y=1,
            color='k',
            linewidth=.5,
            linestyle='--',
        )
        print(df['time_received'].max())

    fig.tight_layout(pad=0.3)
    for file_ending in ["pdf", "pgf"]:
        fig.savefig(f"latency_ecdf.{file_ending}")


def process_ecdf(data_filename: str, num_message: int):
    df = pd.read_csv(data_filename)

    df_plot = (
        df.groupby(['bot_id', 'received_message'])['sim_time_ns']
        .min()
        .to_frame(name='time_received')
        .reset_index(inplace=False)
        .query('received_message == 1')
        .sort_values('time_received')
    )
    df_plot['total_received'] = df_plot.received_message.cumsum()
    df_plot['fraction_received'] = (
        df_plot.total_received
        / df.bot_id.nunique()
    )

    df_plot['time_received'] *= 1e-9  # to seconds
    df_plot['time_received'] -= num_message

    print(df_plot)

    return df_plot


if __name__ == '__main__':
    main()