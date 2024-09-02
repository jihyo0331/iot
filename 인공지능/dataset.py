import pandas as pd
import numpy as np
from datetime import datetime, timedelta

# 가상의 센서 데이터 생성
np.random.seed(42)

def generate_sensor_data(start_date, end_date, num_entries):
    timestamps = pd.date_range(start=start_date, end=end_date, freq='H').to_pydatetime().tolist()
    sensor_data = {
        'timestamp': np.random.choice(timestamps, size=num_entries, replace=True),
        'sensor_id': np.random.choice(['sensor_1', 'sensor_2', 'sensor_3'], size=num_entries),
        'event_type': np.random.choice(['motion', 'door_open', 'light_on'], size=num_entries),
        'location': np.random.choice(['living_room', 'bedroom', 'kitchen'], size=num_entries)
    }
    return pd.DataFrame(sensor_data)

# 데이터 생성
start_date = datetime(2023, 1, 1)
end_date = datetime(2023, 1, 7)
data = generate_sensor_data(start_date, end_date, 1000)

# 타임스탬프 정렬
data = data.sort_values('timestamp').reset_index(drop=True)
print(data.head())

# 타임스탬프에서 시간대(hour)와 요일(day)을 추출
data['hour'] = data['timestamp'].dt.hour
data['day_of_week'] = data['timestamp'].dt.day_name()

# 시간대별 이벤트 발생 빈도
hourly_activity = data.groupby('hour').size()

# 요일별 이벤트 발생 빈도
daily_activity = data.groupby('day_of_week').size()

print(hourly_activity)
print(daily_activity)
