DROP TABLE IF EXISTS sensor_data CASCADE;

CREATE TABLE IF NOT EXISTS sensor_data (
    id SERIAL PRIMARY KEY,
    sensor_value TEXT NOT NULL,
    created_at TIMESTAMP WITH TIME ZONE DEFAULT now()
);

ALTER TABLE sensor_data ENABLE ROW LEVEL SECURITY;

-- Allow all (public) select/insert for anon key (for demo/dev, restrict in production!)
CREATE POLICY "Allow read for all"
  ON sensor_data
  FOR SELECT
  USING (true);

CREATE POLICY "Allow insert for all"
  ON sensor_data
  FOR INSERT
  WITH CHECK (true);

-- Dummy data insert example
INSERT INTO sensor_data (sensor_value)
VALUES
  ('{"heartRate":78,"spo2":98,"temp":36.5}'),
  ('{"heartRate":85,"spo2":97,"temp":36.8}'),
  ('{"heartRate":92,"spo2":96,"temp":37.1}'),
  ('{"heartRate":60,"spo2":99,"temp":36.2}');
