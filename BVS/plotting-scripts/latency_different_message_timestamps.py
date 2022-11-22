import pandas as pd
import matplotlib.pyplot as plt
import matplotlib as mpl

import common #local script

def main():
    # common.set_plotting_style(
    #     style='paper-color',
    #     enable_pgf=True,
    # )
    # figsize = common.get_latex_fig_size(
    #     scale=0.75,
    #     ratio=9/18,
    #     text_width_pt=common.style_default_width_pt,
    #     dpi=common.style_dpi,
    # )
    fig, ax = plt.subplots()#figsize=figsize

    nums_bots = 1000
    nums_message_at = [60 , 240, 420]

    stats = dict()
    legend_lines = []
    legend_labels = []

    plt.xticks(fontsize=14, rotation=0)
    plt.yticks(fontsize=14, rotation=0)

    for i, num_message_at in enumerate(nums_message_at):
        df = process_ecdf(data_filename=f"csvNano_gateway_6500_{num_message_at}.csv", num_message=num_message_at)
        linestyle = '-'  # no dashes in this case
        color = 'midnightblue'
        if i == 1:
            color = 'mediumblue'
        elif i ==2 :
            color = 'darkturquoise'
        elif i == 3:
            color = "cornflowerblue"
        # color = common.style_color_cat_palette[i]
        df.plot(
            x='time_received',
            y='fraction_received',
            ax=ax,
            color=color,
            linestyle=linestyle,
            legend=False,
            linewidth=3
        )
        stats[nums_bots] = dict(
            time_first_received=df.time_received.min(),
            time_fifty_percent_received=(
                df.query('fraction_received > .5').time_received.min()
            ),
            time_ninty_percent_received=(
                df.query('fraction_received > .9').time_received.min()
            ),
            time_last_received=df.time_received.max()
        )
        legend_lines.append(
            mpl.lines.Line2D([0], [0], color=color, linestyle=linestyle)
        )
        legend_labels.append(f"message sent at {num_message_at/60} min",)

        ax.legend(
            legend_lines,
            legend_labels,
            loc='lower right',
            fontsize=14
        )
        ax.set_xlabel("Latency in s", fontsize=15)
        ax.set_ylabel("Fraction of Nanobots reached, ECDF", fontsize=15)
        ax.set_ylim(bottom=0)
        ax.set_xlim(left=0, right=60)
        ax.axhline(
            y=1,
            color='k',
            linewidth=1.0,
            linestyle='--'
        )
        # print(df['time_received'].max())

    fig.tight_layout(pad=0.3)
    for file_ending in ["pdf", "pgf", "png"]:
        fig.savefig(f"latency_ecdf.{file_ending}")

    common.stats_to_latex(
        stats=stats,
        filename="latency_stats.tex",
        cmd_prefix="Nano",
    )

def process_ecdf(data_filename: str, num_message: int):
    df = pd.read_csv(data_filename)
	df.columns = ["bot_id", "x", "y", "z", "sim_time_ns", "vessel_id", "stream_id", "received_message", "hop_count"]

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


    return df_plot


if __name__ == '__main__':
    main()

