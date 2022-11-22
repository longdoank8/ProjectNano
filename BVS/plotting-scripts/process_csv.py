import pandas as pd
import numpy as np

# read file in
file = r'csvNano_hopcount_6500_420_3.csv'
df = pd.read_csv(file)

df['unique_id'] = df.index

df_plot = (
    df.groupby(['bot_id', 'received_message'], as_index=False)
        # ['sim_time_ns'].min()
        .agg({
        'sim_time_ns': 'max',
        'unique_id': 'min', # unique_id should also increase monotonically…
        }
        )
        .query('received_message == 1')
        .sort_values('sim_time_ns')
        .set_index('unique_id')
        .join(df.set_index('unique_id'), lsuffix='_caller')
)



# df_plot['sim_time_ns'] *= 1e-9  # to seconds
# df_plot['vessel_id'] = df.loc[df_plot.index].vessel_id

columns = list(df_plot.head(2))
#
# for i in columns:
#     print(df_plot[i][2])



# resultdf = df_plot.groupby(['sim_time_ns'])[['vessel_id']].nunique()

# df_plot['vessel_id']= df_plot.sort_values('vessel_id')

# print(
#
# "Number of different vessels in which bots "
#
# "first received the message: "
#
# f"{df_plot.vessel_id.nunique()}.\n"
#
# f"The vessels IDs are {df_plot.vessel_id.unique()}"
#
# )
print(df_plot)
print(df_plot['hop_count'].std())

print(df_plot['hop_count'].mean())

# print(resultdf)